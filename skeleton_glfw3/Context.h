#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace je
{
    class Context
    {
    private:
        GLFWwindow* window_;

    public:
        Context(GLuint width, GLuint height, const GLchar* title);
        ~Context();

        GLFWwindow* Window() const { return window_; }
    };
}
