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

        bool ShouldQuit() const { return glfwWindowShouldClose(window_); }
        void SetShouldQuit(bool shouldQuit) { glfwSetWindowShouldClose(window_, shouldQuit ? GLFW_TRUE : GLFW_FALSE); }

        void SwapBuffers() { glfwSwapBuffers(window_); }
    };
}
