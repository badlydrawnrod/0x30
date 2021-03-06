#pragma once
#include "Platform.h"

#include <functional>

namespace je
{
    class Context
    {
    private:
        GLFWwindow* window_;

    public:
        Context(GLuint width, GLuint height, const GLchar* title);
        ~Context();

        GLFWwindow* Window() const
        {
            return window_;
        }

        bool ShouldQuit() const
        {
            return glfwWindowShouldClose(window_);
        }
        void SetShouldQuit(bool shouldQuit)
        {
            glfwSetWindowShouldClose(window_, shouldQuit ? GLFW_TRUE : GLFW_FALSE);
        }

        void SwapBuffers()
        {
            glfwSwapBuffers(window_);
        }

        void Clear()
        {
            // Clear the colour buffer.
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
        {
            // Set the viewport position and size.
            glViewport(x, y, width, height);
        }
    };
} // namespace je
