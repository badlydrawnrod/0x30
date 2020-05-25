#include "Keyboard.h"

#include "Context.h"

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>


namespace je
{
    static void HandleKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (void* userPointer = glfwGetWindowUserPointer(window); userPointer)
        {
            auto context = reinterpret_cast<je::Context*>(userPointer);
            context->KeyboardEventHandler(window, key, scancode, action, mode);
        }
    }

    static GLFWkeyfun keyboardCallback = HandleKeyboardEvents;

    GLFWkeyfun GetKeyboardHandler()
    {
        return keyboardCallback;
    }

    void SetKeyboardHandler(GLFWkeyfun handler)
    {
        keyboardCallback = handler;
    }
}
