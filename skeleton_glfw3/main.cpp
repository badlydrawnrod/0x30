#include "Batch.h"
#include "Logger.h"
#include "Shaders.h"
#include "Textures.h"
#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>


// Window information.
const GLuint WIDTH = 512;
const GLuint HEIGHT = 512;
const char* const TITLE = "GLFW Skeleton App";

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


namespace je
{
    GLFWwindow* InitializeContext()
    {
        // Initialize GLFW.
        glfwInit();

        // Set the options for GLFW.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create a GLFWwindow and make its context current.
        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
        glfwMakeContextCurrent(window);
        if (window == nullptr)
        {
            LOG("Failed to create GLFW window");
            glfwTerminate();
            return nullptr;
        }

        // Load modern OpenGL mappings.
        if (!gladLoadGL())
        {
            LOG("Failed to initialize OpenGL context");
            glfwTerminate();
            return nullptr;
        }
        LOG("Using OpenGL " << GLVersion.major << "." << GLVersion.minor);

        // Enable OpenGL debugging. Requires OpenGL 4.3 or greater.
        if (glDebugMessageCallback)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(OnDebugMessage, nullptr);
        }

        return window;
    }

    void TearDownContext()
    {
        glfwTerminate();
    }
}


int main()
{
    GLFWwindow* window = je::InitializeContext();
    if (!window)
    {
        return -1;
    }

    // Create a shader program.
    auto shaderProgram = je::InitializeShaders();
    LOG("Shader program " << shaderProgram);

    // Wire up some GLFW callbacks.
    glfwSetKeyCallback(window, OnKeyEvent);
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

    {
        je::Batch batch(shaderProgram);

        // Loop.
        while (!glfwWindowShouldClose(window))
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

            // Fill the screen with the texture that we loaded previously.
            je::Quad quad;
            je::Rect2v srcRect;
            srcRect.position = { 0, 0 };
            srcRect.size = { (GLfloat)texture.w, (GLfloat)texture.h };
            je::Vec2f dstSize = { VIRTUAL_WIDTH, VIRTUAL_HEIGHT };
            je::MakeQuad(&texture, srcRect, dstSize, false, false, &quad);
            je::Position position;
            position.position.x = VIRTUAL_WIDTH / 2;
            position.position.y = VIRTUAL_WIDTH / 2;
            position.centre.x = 0.0f;
            position.centre.y = 0.0f;
            position.rotation.cos = 1.0f;
            position.rotation.sin = 0.0f;
            position.scale.x = 1.0f;
            position.scale.y = 1.0f;
            batch.AddQuad(&quad, &position);

            batch.End();

            // Swap buffers.
            glfwSwapBuffers(window);
        }
    }

    je::TearDownShaders();
    je::TearDownContext();

    return 0;
}
