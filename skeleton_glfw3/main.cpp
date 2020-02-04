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


// Window information.
const GLuint WIDTH = 720;
const GLuint HEIGHT = 480;
const char* const TITLE = "The Mysterious 0x30";

// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = WIDTH / 2;
const GLsizei VIRTUAL_HEIGHT = HEIGHT / 2;


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


void GLAPIENTRY OnDebugMessage(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    LOG("GL callback: type = " << type << ", severity = " << severity << ", message = " << message);
}


// Called by GLFW whenever a key is pressed or released.
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        ToggleConsole();
    }
}


// Called by GLFW whenever a joystick / gamepad is connected or disconnected.
void OnJoystickEvent(int joystickId, int event)
{
    if (event == GLFW_CONNECTED)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - connected");
        }
    }
    else if (event == GLFW_DISCONNECTED)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - disconnected");
        }
    }
}


// TODO: the size has to account for things thrown in on top of what's there, and, top be honest, a better mental
// model is a circular buffer of rows. There can only ever be "1 + rows" rows visible, and a blank row that scrolls
// off the top will go off the bottom and get filled in.
struct Pit
{
    Pit();

    static constexpr size_t cols = 6;
    static constexpr size_t rows = 12;
    enum class Tile { None, Red, Green, Yellow, Cyan, Wall };

    std::array<Tile, cols * rows> tiles_;
};


Pit::Pit()
{
    std::fill(tiles_.begin() + cols * 3, tiles_.begin() + cols * 4, Tile::Wall);
    std::fill(tiles_.begin() + cols * rows / 2, tiles_.end(), Tile::Red);
}


int main()
{
    je::Context context(WIDTH, HEIGHT, TITLE);
    if (!context.Window())
    {
        return -1;
    }

    // Enable OpenGL debugging. Requires OpenGL 4.3 or greater.
    if (glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OnDebugMessage, nullptr);
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
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - found");
        }
    }

    // Load a texture.
    je::Texture texture = je::LoadTextureFromFile("../assets/sprite_tiles.png");

    je::Batch batch(shader.Program());

    // Extract some interesting regions from the texture.
    auto redTile = je::TextureRegion{ texture, 0.0f, 48.0f, 16.0f, 16.0f };
    auto greenTile = je::TextureRegion{ texture, 16.0f, 48.0f, 16.0f, 16.0f };
    auto yellowTile = je::TextureRegion{ texture, 32.0f, 48.0f, 16.0f, 16.0f };
    auto magentaTile = je::TextureRegion{ texture, 48.0f, 48.0f, 16.0f, 16.0f };
    auto cyanTile = je::TextureRegion{ texture, 64.0f, 48.0f, 16.0f, 16.0f };
    auto wallTile = je::TextureRegion{ texture, 80.0f, 48.0f, 16.0f, 16.0f };

    Pit pit;
    je::Vec2f topLeft{ VIRTUAL_WIDTH / 2.0f - 3.0f * 16.0f, 32.0f + 16.0f * 8.0f };
    
    const float bottomRow = 32.0f + 16.0f * 12;
    const float lastRow = bottomRow - 16.0f;

    // Loop.
    while (!glfwWindowShouldClose(context.Window()))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        glfwPollEvents();

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the viewport position and size.
        glViewport(0, 0, WIDTH, HEIGHT);

        // Draw the batch.
        batch.Begin(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

        // Draw the pit itself.
        for (auto row = 0; row < pit.rows; row++)
        {
            for (auto col = 0; col < pit.cols; col++)
            {
                je::TextureRegion* drawTile = nullptr;

                switch (pit.tiles_[col + row * pit.cols])
                {
                case Pit::Tile::Red:
                    drawTile = &redTile;
                    break;

                case Pit::Tile::Yellow:
                    drawTile = &yellowTile;
                    break;

                case Pit::Tile::Green:
                    drawTile = &greenTile;
                    break;

                case Pit::Tile::Cyan:
                    drawTile = &cyanTile;
                    break;

                case Pit::Tile::Wall:
                    drawTile = &wallTile;
                    break;

                case Pit::Tile::None:
                default:
                    break;
                }

                if (drawTile)
                {
                    float y = topLeft.y + row * 16.0f;
                    if (y < lastRow)
                    {
                        batch.AddVertices(je::quads::Create(*drawTile, topLeft.x + col * 16.0f, topLeft.y + row * 16.0f));
                    }
                    else if (y < bottomRow)
                    {
                        je::Rgba4b grey{ 0x6f, 0x6f, 0x6f, 0xff };
                        batch.AddVertices(je::quads::Create(*drawTile, topLeft.x + col * 16.0f, topLeft.y + row * 16.0f, grey));
                    }
                }
            }
        }

        // Draw a rather rudimentary looking outline of a pit.
        for (int y = 0; y < 13; y++)
        {
            batch.AddVertices(je::quads::Create(wallTile, VIRTUAL_WIDTH / 2.0f - 4.0f * 16.0f, 32.0f + 16.0f * y));
            batch.AddVertices(je::quads::Create(wallTile, VIRTUAL_WIDTH / 2.0f + 3.0f * 16.0f, 32.0f + 16.0f * y));
        }
        for (int x = 1; x < 7; x++)
        {
            batch.AddVertices(je::quads::Create(wallTile, VIRTUAL_WIDTH / 2.0f - 4.0f * 16.0f + x * 16.0f, 32.0f + 16.0f * 12));
        }

        topLeft.y -= 0.1f;

        batch.End();

        // Swap buffers.
        glfwSwapBuffers(context.Window());
    }

    return 0;
}
