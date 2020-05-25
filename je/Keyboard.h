#pragma once

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace je
{
    GLFWkeyfun GetKeyboardHandler();
    void SetKeyboardHandler(GLFWkeyfun handler);
}
