#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>



// Window dimensions.
const GLuint WIDTH = 640;
const GLuint HEIGHT = 480;
const char* const TITLE = "GLFW Skeleton App";


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


int main()
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
        return -1;
    }

    // Load modern OpenGL mappings.
    if (!gladLoadGL())
    {
        LOG("Failed to initialize OpenGL context");
        return -1;
    }
    LOG("Using OpenGL " << GLVersion.major << "." << GLVersion.minor);

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

    // Set the viewport size.
    glViewport(0, 0, WIDTH, HEIGHT);

    // Loop.
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers.
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing its resources.
    glfwTerminate();

    return 0;
}
