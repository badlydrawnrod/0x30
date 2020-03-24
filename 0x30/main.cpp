#include "Assets.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "TextRenderer.h"

#include "je/Batch.h"
#include "je/Context.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Shaders.h"
#include "je/Textures.h"
#include "je/Time.h"
#include "je/Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>


// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = 320;
const GLsizei VIRTUAL_HEIGHT = 240;

// Window information.
const GLuint WIDTH = VIRTUAL_WIDTH * 3;
const GLuint HEIGHT = VIRTUAL_HEIGHT * 3;

const char* const TITLE = "The Mysterious 0x30";

// Input states.
bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;
bool swapPressed = false;
bool fillHeld = false;

bool keyboardFillHeld = false;

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

    bool isPress = (action == GLFW_PRESS);
    bool isPressOrRepeat = (action == GLFW_PRESS || action == GLFW_REPEAT);
    switch (key)
    {
    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
        leftPressed = isPress;
        break;
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
        rightPressed = isPress;
        break;
    case GLFW_KEY_UP:
    case GLFW_KEY_W:
        upPressed = isPress;
        break;
    case GLFW_KEY_DOWN:
    case GLFW_KEY_S:
        downPressed = isPress;
        break;
    case GLFW_KEY_SPACE:
    case GLFW_KEY_ENTER:
    case GLFW_KEY_Z:
    case GLFW_KEY_X:
    case GLFW_KEY_C:
        swapPressed = isPress;
        break;
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_RIGHT_CONTROL:
        keyboardFillHeld = isPressOrRepeat;
        break;
    default:
        break;
    }
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

    // Reset the state.
    leftPressed = false;
    rightPressed = false;
    upPressed = false;
    downPressed = false;
    swapPressed = false;

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
        fillHeld = keyboardFillHeld || state.buttons[GLFW_GAMEPAD_BUTTON_X];

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


class TimeRenderer
{
public:
    TimeRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position, double elapsed);

private:
    TextRenderer& textRenderer_;
    int minutes_;
    int seconds_;
    char timeBuf_[128] = { 0 };
    int numChars_;
};


TimeRenderer::TimeRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }, minutes_{ -1 }, seconds_{ -1 }, numChars_{ 0 }
{
}


void TimeRenderer::Draw(je::Vec2f position, double elapsed)
{
    int minutes = (int)(elapsed / 60.0);
    int seconds = ((int)elapsed % 60);

    // Only re-create the string if it has changed.
    if (minutes != minutes_ || seconds != seconds_)
    {
        numChars_ = sprintf_s(timeBuf_, sizeof(timeBuf_), "%d'%02d", minutes, seconds);
        minutes_ = minutes;
        seconds_ = seconds;
    }

    // TODO: juice it up.
    // Draw "TIME" on one row, with the elapsed time on the following row, right-justified to "TIME".
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b timeColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "TIME", blackColour);
    textRenderer_.Draw(position.x, position.y, "TIME", textColour);
    // TODO: lose the magic numbers.
    textRenderer_.Draw(1.0f + position.x + 32.0f - 8.0f * numChars_, 1.0f + position.y + 10.0f, timeBuf_, blackColour);
    textRenderer_.Draw(position.x + 32.0f - 8.0f * numChars_, position.y + 10.0f, timeBuf_, timeColour);
}


class ScoreRenderer
{
public:
    ScoreRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position, uint64_t score);

private:
    TextRenderer textRenderer_;
};


ScoreRenderer::ScoreRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }
{
}


void ScoreRenderer::Draw(je::Vec2f position, uint64_t score)
{
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b scoreColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "SCORE", blackColour);
    textRenderer_.Draw(position.x, position.y, "SCORE", textColour);
    std::ostringstream scoreString;
    scoreString << std::setw(8) << score;
    textRenderer_.Draw(1.0f + position.x - 24.0f, 1.0f + position.y + 10.0f, scoreString.str(), blackColour);
    textRenderer_.Draw(position.x - 24.0f, position.y + 10.0f, scoreString.str(), scoreColour);
}


class SpeedRenderer
{

public:
    SpeedRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position);

private:
    TextRenderer textRenderer_;
};


SpeedRenderer::SpeedRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }
{
}


void SpeedRenderer::Draw(je::Vec2f position)
{
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b speedColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "SPEED", blackColour);
    textRenderer_.Draw(position.x, position.y, "SPEED", textColour);
    textRenderer_.Draw(1.0f + position.x + 32.0f, 1.0f + position.y + 10.0f, "2", blackColour);
    textRenderer_.Draw(position.x + 32.0f, position.y + 10.0f, "2", speedColour);
}


class Flyup
{
public:
    Flyup(je::TextureRegion& texture, float x, float y, float duration);
    Flyup(const Flyup& other);
    Flyup& operator=(const Flyup& other);

    void Draw(je::Batch& batch);
    bool IsAlive() const;

private:
    je::TextureRegion texture_;
    float x_;
    float y_;
    double endTime_;
};


Flyup::Flyup(je::TextureRegion& texture, float x, float y, float duration) : texture_{ texture }, x_{ x }, y_{ y }, endTime_{ je::GetTime() + duration }
{
}

Flyup::Flyup(const Flyup& other) : texture_{ other.texture_ }, x_{ other.x_ }, y_{ other.y_ }, endTime_{ other.endTime_ }
{
}

Flyup& Flyup::operator=(const Flyup& other)
{
    texture_ = other.texture_;
    x_ = other.x_;
    y_ = other.y_;
    endTime_ = other.endTime_;

    return *this;
}



void Flyup::Draw(je::Batch& batch)
{
    batch.AddVertices(je::quads::Create(texture_, x_, y_));
    y_ -= 0.25f;
}


bool Flyup::IsAlive() const
{
    return je::GetTime() < endTime_;
}


void UpdateScore(const Pit& pit, uint64_t& score)
{
    const auto& runs = pit.Runs();
    if (runs.size() > 0)
    {
        auto multiplier = runs.size();
        LOG("Run info");
        int n = 1;
        for (auto runInfo : runs)
        {
            LOG(n << " size: " << runInfo.runSize << ", chain: " << runInfo.chainLength);
            ++n;
            int runScore = 0;
            switch (runInfo.runSize)
            {
            case 3:
                runScore = 10;
                break;
            case 4:
                runScore = 25;
                break;
            case 5:
                runScore = 50;
                break;
            case 6:
                runScore = 100;
                break;
            case 7:
                runScore = 250;
                break;
            case 8:
                runScore = 500;
                break;
            case 9:
                runScore = 1000;
                break;
            default:
                break;
            }
            uint64_t scoreChange = runScore * (runInfo.chainLength + 1) * multiplier;
            LOG("Run score: " << runScore << " * chain length " << (runInfo.chainLength + 1) << " * multiplier " << multiplier << " = " << scoreChange);
            score += scoreChange;
        }
        LOG("Score: " << score);
    }
}


void AddFlyupsForRun(std::vector<Flyup>& flyups, const Pit::RunInfo& run, const Textures& textures, je::Vec2f topLeft, float internalTileScroll, float tileSize)
{
    // Add fly-ups for runs of 4-9.
    if (run.runSize >= 4 && run.runSize < 10)
    {
        float runFlyupDuration = 1.0f;
        je::TextureRegion texture;

        switch (run.runSize)
        {
        case 4:
            texture = textures.combo4;
            break;
        case 5:
            texture = textures.combo5;
            break;
        case 6:
            texture = textures.combo6;
            break;
        case 7:
            texture = textures.combo7;
            break;
        case 8:
            texture = textures.combo8;
            break;
        case 9:
            texture = textures.combo9;
            break;
        }

        for (auto i = 0; i < run.runSize; i++)
        {
            float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
            float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll;
            flyups.emplace_back(texture, x, y, runFlyupDuration);
        }
    }
}


void AddFlyupsForChains(std::vector<Flyup>& flyups, const Pit::RunInfo& run, const Textures& textures, je::Vec2f topLeft, float internalTileScroll, float tileSize)
{
    // Add fly-ups for chains of 2-6.
    if (const auto chains = run.chainLength + 1; chains >= 2 && chains < 7)
    {
        const float chainFlyupDuration = 1.5f;
        je::TextureRegion texture;
        switch (chains)
        {
        case 2:
            texture = textures.chain2;
            break;
        case 3:
            texture = textures.chain3;
            break;
        case 4:
            texture = textures.chain4;
            break;
        case 5:
            texture = textures.chain5;
            break;
        case 6:
            texture = textures.chain6;
            break;
        }

        for (auto i = 0; i < run.runSize; i++)
        {
            float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
            float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll - tileSize;
            flyups.emplace_back(texture, x, y, chainFlyupDuration);
        }
    }
}


class Playing
{
public:
    Playing(je::Batch& batch, Textures& textures, std::function<int(int, int)>& rnd);

    void Update();
    void Draw();

private:
    const float tileSize = 16.0f;

    je::Batch& batch;
    Textures& textures;
    Pit pit;
    PitRenderer pitRenderer;
    TextRenderer textRenderer;
    TimeRenderer timeRenderer;
    ScoreRenderer scoreRenderer;
    SpeedRenderer speedRenderer;
    double startTime{ je::GetTime() };

    const je::Vec2f topLeft{ (VIRTUAL_WIDTH - Pit::cols * tileSize) / 2.0f, VIRTUAL_HEIGHT - Pit::rows * tileSize };
    const float bottomRow{ topLeft.y + (Pit::rows - 1) * tileSize };
    const float lastRow{ bottomRow - tileSize };

    float internalTileScroll{ 0.0f };
    float scrollRate{ 0.025f };

    int cursorTileX{ (Pit::cols / 2) - 1 };
    int cursorTileY{ Pit::rows / 2 };

    size_t counter{ 0 };
    uint64_t score{ 0 };
    std::vector<Flyup> flyups;
};


Playing::Playing(je::Batch& batch, Textures& textures, std::function<int(int, int)>& rnd) :
    batch{ batch },
    textures{ textures },
    pit{ rnd },
    pitRenderer{ pit, textures, batch },
    textRenderer{ textures.textTiles, batch },
    timeRenderer{ textRenderer },
    scoreRenderer{ textRenderer },
    speedRenderer{ textRenderer }
{
}


void Playing::Update()
{
    double now = je::GetTime();
    double elapsed = now - startTime;

    if (!pit.IsImpacted())
    {
        // Scroll the contents of the pit up.
        internalTileScroll += fillHeld ? 1.0f : scrollRate;
        if (internalTileScroll >= tileSize)
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

        pit.Update();

        UpdateScore(pit, score);

        // Add fly-ups.
        for (const auto& run : pit.Runs())
        {
            AddFlyupsForRun(flyups, run, textures, topLeft, internalTileScroll, tileSize);
            AddFlyupsForChains(flyups, run, textures, topLeft, internalTileScroll, tileSize);
        }

        // Remove dead fly-ups.
        flyups.erase(std::remove_if(flyups.begin(), flyups.end(), [](const auto& f) { return !f.IsAlive(); }), flyups.end());

        // Draw some stats.
        batch.AddVertices(je::quads::Create(textures.blankTile, topLeft.x - tileSize * 3 - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 3, tileSize * 2));
        batch.AddVertices(je::quads::Create(textures.blankTile, topLeft.x + tileSize * (pit.cols + 1) - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 5, tileSize * 4));
        timeRenderer.Draw({ topLeft.x - tileSize * 3, topLeft.y + tileSize * 2 }, elapsed);
        scoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 2 }, score);
        speedRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 4 });
    }
}


void Playing::Draw()
{
    // Draw the backdrop.
    batch.AddVertices(je::quads::Create(textures.backdrops[3], 0.0f, 0.0f));

    batch.AddVertices(je::quads::Create(textures.blankTile, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * pit.rows));
    pitRenderer.Draw(topLeft, internalTileScroll, lastRow, bottomRow);

    if (!pit.IsImpacted())
    {
        // We're still playing, so draw the cursor.
        float cursorX = topLeft.x + cursorTileX * tileSize - 1.0f;
        float cursorY = topLeft.y + cursorTileY * tileSize - 1.0f - internalTileScroll;
        batch.AddVertices(je::quads::Create(textures.cursorTile, cursorX, cursorY));
        batch.AddVertices(je::quads::Create(textures.cursorTile, cursorX + tileSize, cursorY));
    }
    else
    {
        // It's game over, so tell the player.
        if (counter % 60 < 40)
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 40.0f;
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
            textRenderer.Draw(x + 1.0f, y + 1.0f, "GAME OVER!", { 0x00, 0x00, 0x00, 0xff });
            textRenderer.Draw(x, y, "GAME OVER!");
        }
        counter++;
    }

    // Draw fly-ups.
    for (auto& flyup : flyups)
    {
        if (flyup.IsAlive())
        {
            flyup.Draw(batch);
        }
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

    // Make a function to create random integers in a closed range.
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::function<int(int, int)> Rnd = [&](int lo, int hi) {
        std::uniform_int_distribution<int> distribution(lo, hi);
        return distribution(generator);
    };

    Textures textures;
    Playing playing(batch, textures, Rnd);

    // Loop.
    while (!glfwWindowShouldClose(context.Window()))
    {
        UpdateInputState();
        playing.Update();

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the viewport position and size.
        glViewport(0, 0, WIDTH, HEIGHT);

        // Draw the batch.
        batch.Begin(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        playing.Draw();
        batch.End();

        // Swap buffers.
        glfwSwapBuffers(context.Window());
    }

    return 0;
}
