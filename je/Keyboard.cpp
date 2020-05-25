#include "Keyboard.h"

#include "Human.h"

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
            auto human = reinterpret_cast<je::Human*>(userPointer);
            human->KeyboardEventHandler(window, key, scancode, action, mode);
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
