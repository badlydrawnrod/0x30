#include "Batch.h"
#include "Context.h"
#include "Logger.h"
#include "QuadHelpers.h"
#include "Shaders.h"
#include "Textures.h"
#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <random>


// Window information.
const GLuint WIDTH = 1280;
const GLuint HEIGHT = 720;
const char* const TITLE = "The Mysterious 0x30";

// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = WIDTH / 3;
const GLsizei VIRTUAL_HEIGHT = HEIGHT / 3;

// Input states.
bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;
bool swapPressed = false;

bool wasLeftPressed = false;
bool wasRightPressed = false;
bool wasUpPressed = false;
bool wasDownPressed = false;
bool wasSwapPressed = false;

bool leftActivated = false;
bool rightActivated = false;
bool upActivated = false;
bool downActivated = false;

bool wasLeftActivated = false;
bool wasRightActivated = false;
bool wasUpActivated = false;
bool wasDownActivated = false;

float joystickX = 0.0f;
float joystickY = 0.0f;
float oldJoystickX = 0.0f;
float oldJoystickY = 0.0f;


// Show / hide the Windows console.
void ToggleConsole()
{
    static bool isConsoleHidden = false;

    if (isConsoleHidden)
    {
        HWND activeWindow = GetActiveWindow();
        ShowWindow(GetConsoleWindow(), 1);
        SetFocus(activeWindow);
    }
    else
    {
        ShowWindow(GetConsoleWindow(), 0);
    }
    isConsoleHidden = !isConsoleHidden;
}


// Called by GLFW whenever a key is pressed or released.
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
    {
        ToggleConsole();
    }

    leftPressed = action == GLFW_PRESS && (key == GLFW_KEY_A || key == GLFW_KEY_LEFT);
    rightPressed = action == GLFW_PRESS && (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT);
    upPressed = action == GLFW_PRESS && (key == GLFW_KEY_W || key == GLFW_KEY_UP);
    downPressed = action == GLFW_PRESS && (key == GLFW_KEY_S || key == GLFW_KEY_DOWN);
    swapPressed = action == GLFW_PRESS && (key == GLFW_KEY_SPACE || key == GLFW_KEY_ENTER || key == GLFW_KEY_Z || key == GLFW_KEY_X || key == GLFW_KEY_C);
}


// Called by GLFW whenever a joystick / gamepad is connected or disconnected.
void OnJoystickEvent(int joystickId, int event)
{
    // We only care if the joystick in question is a gamepad.
    if (glfwJoystickIsGamepad(joystickId))
    {
        if (event == GLFW_CONNECTED)
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - connected");
        }
        else if (event == GLFW_DISCONNECTED)
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - disconnected");
        }
    }
}


void UpdateInputState()
{
    // Copy the current state.
    wasLeftPressed = leftPressed;
    wasRightPressed = rightPressed;
    wasUpPressed = upPressed;
    wasDownPressed = downPressed;
    wasSwapPressed = swapPressed;
    oldJoystickX = joystickX;
    oldJoystickY = joystickY;
    wasLeftActivated = leftActivated;
    wasRightActivated = rightActivated;
    wasUpActivated = upActivated;
    wasDownActivated = downActivated;

    // Reset the current stat before polling it.
    leftPressed = false;
    rightPressed = false;
    upPressed = false;
    downPressed = false;
    swapPressed = false;
    joystickX = 0.0f;
    joystickY = 0.0f;
    leftActivated = false;
    rightActivated = false;
    upActivated = false;
    downActivated = false;

    // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
    glfwPollEvents();

    // Poll the first gamepad.
    GLFWgamepadstate state;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        leftPressed = leftPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
        rightPressed = rightPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
        upPressed = upPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
        downPressed = downPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
        swapPressed = swapPressed || state.buttons[GLFW_GAMEPAD_BUTTON_A];

        joystickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        joystickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

        const float threshold = 0.5f;
        leftActivated = joystickX < -threshold;
        rightActivated = joystickX > threshold;
        upActivated = joystickY < -threshold;
        downActivated = joystickY > threshold;

        leftPressed = leftPressed || (leftActivated && !wasLeftActivated);
        rightPressed = rightPressed || (rightActivated && !wasRightActivated);
        upPressed = upPressed || (upActivated && !wasUpActivated);
        downPressed = downPressed || (downActivated && !wasDownActivated);
    }
}


class Pit
{
public:
    static constexpr size_t cols = 6;
    static constexpr size_t rows = 13;  // Note, one more than is visible because of the wraparound.

    enum class Tile { None, Red, Green, Yellow, Cyan, Magenta, Wall };

public:
    Pit(std::function<int(int, int)>& rnd);

    void ApplyGravity();
    void FindVerticalRuns(bool& foundRun);
    void FindHorizontalRuns(bool& foundRun);
    void FindAdjacentVerticalRuns(bool& foundRun);
    void FindAdjacentHorizontalRuns(bool& foundRun);
    void CheckForRuns();
    void RemoveRuns();
    void RefillBottomRow();
    void ScrollOne();

    void Swap(size_t x, size_t y);
    Tile TileAt(size_t x, size_t y) const;
    size_t PitIndex(size_t x, size_t y) const;

private:
    std::array<Tile, cols * rows> tiles_;
    std::array<bool, cols * rows> runs_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
};


void Pit::RefillBottomRow()
{
    auto start = PitIndex(0, rows - 1);
    auto end = PitIndex(cols - 1, rows - 1);
    Tile pieces[] = {
        Tile::Cyan,
        Tile::Green,
        Tile::Magenta,
        Tile::Red,
        Tile::Yellow
    };
    for (auto i = start; i <= end; i += 2)
    {
        tiles_[i] = pieces[rnd_(0, 4)];
        tiles_[i + 1] = pieces[rnd_(0, 4)];
    }
}


void Pit::ScrollOne()
{
    firstRow_ = (firstRow_ + 1) % rows;
    RefillBottomRow();
}


void Pit::Swap(size_t x, size_t y)
{
    auto& tile1 = tiles_[PitIndex(x, y)];
    auto& tile2 = tiles_[PitIndex(x + 1, y)];
    if (tile1 != Pit::Tile::Wall && tile2 != Pit::Tile::Wall)
    {
        std::swap(tile1, tile2);
    }
}


Pit::Tile Pit::TileAt(size_t x, size_t y) const
{
    return tiles_[PitIndex(x, y)];
}


size_t Pit::PitIndex(size_t x, size_t y) const
{
    size_t col = x % cols;
    size_t row = (y + firstRow_) % rows;
    return col + row * cols;
}


Pit::Pit(std::function<int(int, int)>& rnd) : rnd_(rnd)
{
    std::fill(tiles_.begin(), tiles_.begin() + cols * 1, Tile::Wall);
    std::fill(tiles_.begin() + cols * 1, tiles_.begin() + cols * 4, Tile::Red);
    std::fill(tiles_.begin() + cols * 4, tiles_.begin() + cols * 6, Tile::Green);
    std::fill(tiles_.begin() + cols * 6, tiles_.begin() + cols * 8, Tile::Yellow);
    std::fill(tiles_.begin() + cols * 8, tiles_.begin() + cols * 10, Tile::Cyan);
    std::fill(tiles_.begin() + cols * 10, tiles_.begin() + cols * 13, Tile::Magenta);
    for (size_t y = 0; y < rows; y++)
    {
        if (tiles_[y * cols + 3] != Tile::Wall)
        {
            tiles_[y * cols + 3] = Tile::None;
        }
        if (tiles_[y * cols + 1] != Tile::Wall)
        {
            tiles_[y * cols + 1] = Tile::None;
        }
        for (size_t x = 2; x < 5; x += 2)
        {
            switch (tiles_[y * cols + x])
            {
            case Tile::Wall:
                break;
            case Tile::Cyan:
                tiles_[y * cols + x] = Tile::Magenta;
                break;
            default:
                tiles_[y * cols + x] = Tile::Cyan;
                break;
            }
        }
    }
    std::fill(tiles_.begin() + cols * 11, tiles_.begin() + cols * 12, Tile::Red);

    std::fill(runs_.begin(), runs_.end(), false);
}


void Pit::ApplyGravity()
{
    for (size_t y = rows - 2; y != 0; y--)
    {
        size_t rowAbove = (y + rows - 1 + firstRow_) % rows;
        size_t row = (y + firstRow_) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            if (tiles_[x + row * cols] == Pit::Tile::None)
            {
                if (tiles_[x + rowAbove * cols] != Pit::Tile::None && tiles_[x + rowAbove * cols] != Pit::Tile::Wall)
                {
                    std::swap(tiles_[x + rowAbove * cols], tiles_[x + row * cols]);
                }
            }
        }
    }
}


void Pit::FindVerticalRuns(bool& foundRun)
{
    for (size_t y = 0; y < rows - 3; y++)
    {
        size_t row0 = (y + firstRow_) % rows;
        size_t row1 = (row0 + 1) % rows;
        size_t row2 = (row0 + 2) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            auto tile0 = tiles_[x + row0 * cols];
            auto tile1 = tiles_[x + row1 * cols];
            auto tile2 = tiles_[x + row2 * cols];
            if (tile0 == tile1 && tile1 == tile2)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[x + row0 * cols] = true;
                    runs_[x + row1 * cols] = true;
                    runs_[x + row2 * cols] = true;
                }
            }
        }
    }
}


void Pit::FindHorizontalRuns(bool& foundRun)
{
    // Check for 3 adjacent tiles horizontally.
    for (size_t x = 0; x < cols - 2; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            size_t row = (y + firstRow_) % rows;
            auto tile0 = tiles_[(x + 0) + row * cols];
            auto tile1 = tiles_[(x + 1) + row * cols];
            auto tile2 = tiles_[(x + 2) + row * cols];
            if (tile0 == tile1 && tile1 == tile2)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[(x + 0) + row * cols] = true;
                    runs_[(x + 1) + row * cols] = true;
                    runs_[(x + 2) + row * cols] = true;
                }
            }
        }
    }
}


void Pit::FindAdjacentVerticalRuns(bool& foundRun)
{
    // Look for tiles vertically adjacent to an existing run.
    for (size_t y = 0; y < rows - 2; y++)
    {
        size_t row0 = (y + firstRow_) % rows;
        size_t row1 = (row0 + 1) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            auto tile0 = tiles_[x + row0 * cols];
            auto tile1 = tiles_[x + row1 * cols];
            auto run0 = runs_[x + row0 * cols];
            auto run1 = runs_[x + row1 * cols];
            if (run0 != run1 && tile0 == tile1)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[x + row0 * cols] = true;
                    runs_[x + row1 * cols] = true;
                }
            }
        }
    }
}


void Pit::FindAdjacentHorizontalRuns(bool& foundRun)
{
    // Look for tiles horizontally adjacent to an existing run.
    for (size_t x = 0; x < cols - 1; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            size_t row = (y + firstRow_) % rows;
            auto tile0 = tiles_[(x + 0) + row * cols];
            auto tile1 = tiles_[(x + 1) + row * cols];
            auto run0 = runs_[(x + 0) + row * cols];
            auto run1 = runs_[(x + 1) + row * cols];
            if (run0 != run1 && tile0 == tile1)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[(x + 0) + row * cols] = true;
                    runs_[(x + 1) + row * cols] = true;
                }
            }
        }
    }
}


void Pit::CheckForRuns()
{
    // Look for runs of tiles of the same colour that are at least 3 tiles horizontally or vertically.
    // TODO: Don't include falling tiles, i.e., those that have a blank square below them.

    // At the start, there are no runs.
    std::fill(runs_.begin(), runs_.end(), false);

    // Check for 3 adacent tiles vertically and horizontally.
    bool foundRun = false;
    FindVerticalRuns(foundRun);
    FindHorizontalRuns(foundRun);

    // If we've found any horizontal or vertical runs then look for tiles of the same colour adjacent to a run and
    // add them to it, until we find no further runs.
    while (foundRun)
    {
        foundRun = false;
        FindAdjacentVerticalRuns(foundRun);
        FindAdjacentHorizontalRuns(foundRun);
    }
}


void Pit::RemoveRuns()
{
    for (size_t y = 0; y < rows; y++)
    {
        size_t row = (y + firstRow_) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            if (runs_[x + row * cols])
            {
                tiles_[x + row * cols] = Pit::Tile::None;
            }
        }
    }
}


struct Textures
{
    static constexpr float tile_size = 16.0f;

    Textures()
    {
        texture = je::LoadTextureFromFile("../assets/sprite_tiles.png");
        redTile = je::TextureRegion{ texture, 0 * tile_size, 48.0f, tile_size, tile_size };
        greenTile = je::TextureRegion{ texture, 1 * tile_size, 48.0f, tile_size, tile_size };
        yellowTile = je::TextureRegion{ texture, 2 * tile_size, 48.0f, tile_size, tile_size };
        magentaTile = je::TextureRegion{ texture, 3 * tile_size, 48.0f, tile_size, tile_size };
        cyanTile = je::TextureRegion{ texture, 4 * tile_size, 48.0f, tile_size, tile_size };
        wallTile = je::TextureRegion{ texture, 5 * tile_size, 48.0f, tile_size, tile_size };
        cursorTile = je::TextureRegion{ texture, 103.0f, 47.0f, 17.0f, 17.0f };
    }

    je::Texture texture;
    je::TextureRegion redTile;
    je::TextureRegion greenTile;
    je::TextureRegion yellowTile;
    je::TextureRegion magentaTile;
    je::TextureRegion cyanTile;
    je::TextureRegion wallTile;
    je::TextureRegion cursorTile;
};


class PitRenderer
{
public:
    PitRenderer(const Pit& apit, const Textures& atextures, je::Batch& batch) : pit_{ apit }, textures_{ atextures }, batch{ batch }
    {
    }

    void Draw(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow);
    void DrawContents(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow);
    void DrawOutline(je::Vec2f topLeft);
    const je::TextureRegion* TileAt(size_t col, size_t row) const;

private:
    const Pit& pit_;
    const Textures& textures_;
    je::Batch& batch;
};


void PitRenderer::Draw(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow)
{
    DrawContents(topLeft, internalTileScroll, lastRow, bottomRow);
    DrawOutline(topLeft);
}


void PitRenderer::DrawContents(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow)
{
    // Draw the contents of the pit.
    for (auto row = 0; row < Pit::rows; row++)
    {
        for (auto col = 0; col < Pit::cols; col++)
        {
            const je::TextureRegion* drawTile = TileAt(col, row);
            if (drawTile)
            {
                float y = topLeft.y + row * drawTile->h - internalTileScroll;
                if (y < lastRow)
                {
                    batch.AddVertices(je::quads::Create(*drawTile, topLeft.x + col * drawTile->w, topLeft.y + row * drawTile->h - internalTileScroll));
                }
                else if (y < bottomRow)
                {
                    // Fade in the last row.
                    GLubyte c = (GLubyte)0x3f;
                    switch (int(internalTileScroll))
                    {
                    case 15:
                    case 14:
                        c = (GLubyte)0xef;
                        break;
                    case 13:
                    case 12:
                        c = (GLubyte)0xdf;
                        break;
                    case 11:
                    case 10:
                        c = (GLubyte)0xcf;
                        break;
                    case 9:
                    case 8:
                        c = (GLubyte)0xbf;
                        break;
                    case 7:
                    case 6:
                        c = (GLubyte)0xaf;
                        break;
                    case 5:
                    case 4:
                        c = (GLubyte)0x9f;
                        break;
                    case 3:
                    case 2:
                        c = (GLubyte)0x8f;
                        break;
                    case 1:
                        c = (GLubyte)0x7f;
                        break;
                    default:
                        c = (GLubyte)0xff;
                        break;
                    }
                    je::Rgba4b grey{ c, c, c, 0xff };
                    batch.AddVertices(je::quads::Create(*drawTile, topLeft.x + col * drawTile->w, topLeft.y + row * drawTile->h - internalTileScroll, grey));
                }
            }
        }
    }
}


const je::TextureRegion* PitRenderer::TileAt(size_t col, size_t row) const
{
    switch (pit_.TileAt(col, row))
    {
    case Pit::Tile::Red:
        return &textures_.redTile;

    case Pit::Tile::Yellow:
        return &textures_.yellowTile;

    case Pit::Tile::Green:
        return &textures_.greenTile;

    case Pit::Tile::Cyan:
        return &textures_.cyanTile;

    case Pit::Tile::Magenta:
        return &textures_.magentaTile;

    case Pit::Tile::Wall:
        return &textures_.wallTile;

    case Pit::Tile::None:
    default:
        return nullptr;
    }
}


void PitRenderer::DrawOutline(je::Vec2f topLeft)
{
    // Draw the outline of the pit.
    const auto& wallTile = textures_.wallTile;
    for (int y = 0; y < Pit::rows; y++)
    {
        batch.AddVertices(je::quads::Create(wallTile, topLeft.x - wallTile.w, topLeft.y + wallTile.h * y));
        batch.AddVertices(je::quads::Create(wallTile, topLeft.x + Pit::cols * wallTile.w, topLeft.y + wallTile.h * y));
    }
    for (int x = 0; x < Pit::cols + 2; x++)
    {
        batch.AddVertices(je::quads::Create(wallTile, topLeft.x - wallTile.w + x * wallTile.w, topLeft.y - wallTile.h));
        batch.AddVertices(je::quads::Create(wallTile, topLeft.x - wallTile.w + x * wallTile.w, topLeft.y + wallTile.h * (Pit::rows - 1)));
    }
}


int main()
{
    je::Context context(WIDTH, HEIGHT, TITLE);
    if (!context.Window())
    {
        return -1;
    }

    // Create a shader program.
    je::Shader shader;
    LOG("Shader program " << shader.Program());

    // Wire up some GLFW callbacks.
    glfwSetKeyCallback(context.Window(), OnKeyEvent);
    glfwSetJoystickCallback(OnJoystickEvent);

    // Enumerate the gamepads.
    for (int joystickId = GLFW_JOYSTICK_1; joystickId <= GLFW_JOYSTICK_LAST; joystickId++)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " [" << glfwGetGamepadName(joystickId) << "] found");
        }
    }

    je::Batch batch(shader.Program());

    constexpr float tile_size = 16.0f;

    // Make a function to create random integers in a closed range.
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::function<int(int, int)> Rnd = [&](int lo, int hi) {
        std::uniform_int_distribution<int> distribution(lo, hi);
        return distribution(generator);
    };

    Textures textures;
    Pit pit(Rnd);
    PitRenderer pitRenderer(pit, textures, batch);

    je::Vec2f topLeft{ (VIRTUAL_WIDTH - Pit::cols * tile_size) / 2.0f, VIRTUAL_HEIGHT - Pit::rows * tile_size };

    const float bottomRow = topLeft.y + (Pit::rows - 1) * tile_size;
    const float lastRow = bottomRow - tile_size;

    float internalTileScroll = 0.0f;
    float scrollRate = 0.025f;

    int cursorTileX = (Pit::cols / 2) - 1;
    int cursorTileY = Pit::rows / 2;

    // Loop.
    while (!glfwWindowShouldClose(context.Window()))
    {
        UpdateInputState();

        // Scroll the contents of the pit up.
        internalTileScroll += scrollRate;
        if (internalTileScroll >= tile_size)
        {
            pit.ScrollOne();
            if (cursorTileY > 1)
            {
                cursorTileY--;
            }
            internalTileScroll = 0.0f;
        }

        // Move the player.
        if (leftPressed && !wasLeftPressed && cursorTileX > 0)
        {
            --cursorTileX;
        }
        if (rightPressed && !wasRightPressed && cursorTileX < Pit::cols - 2)
        {
            ++cursorTileX;
        }
        if (upPressed && !wasUpPressed && cursorTileY > 1)
        {
            --cursorTileY;
        }
        if (downPressed && !wasDownPressed && cursorTileY < Pit::rows - 2)
        {
            ++cursorTileY;
        }

        // Swap tiles.
        if (swapPressed && !wasSwapPressed)
        {
            pit.Swap(cursorTileX, cursorTileY);
        }

        pit.ApplyGravity();
        pit.CheckForRuns();
        pit.RemoveRuns();

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the viewport position and size.
        glViewport(0, 0, WIDTH, HEIGHT);

        // Draw the batch.
        batch.Begin(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

        pitRenderer.Draw(topLeft, internalTileScroll, lastRow, bottomRow);

        // Draw the cursor.
        float cursorX = topLeft.x + cursorTileX * tile_size - 1.0f;
        float cursorY = topLeft.y + cursorTileY * tile_size - 1.0f - internalTileScroll;
        batch.AddVertices(je::quads::Create(textures.cursorTile, cursorX, cursorY));
        batch.AddVertices(je::quads::Create(textures.cursorTile, cursorX + tile_size, cursorY));

        batch.End();

        // Swap buffers.
        glfwSwapBuffers(context.Window());
    }

    return 0;
}
