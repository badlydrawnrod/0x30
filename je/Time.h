#pragma once

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>


namespace je
{
    inline double GetTime()
    {
        return glfwGetTime();
    }
}
