#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    inline double GetTime()
    {
        return glfwGetTime();
    }
}
