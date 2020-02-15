#include "Pit.h"

#include "je/Batch.h"
#include "je/Context.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Shaders.h"
#include "je/Textures.h"
#include "je/Types.h"

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
