#include "Context.h"

#include "Logger.h"


namespace je
{
    Context::Context(GLuint width, GLuint height, const GLchar* title)
    {
        // Initialize GLFW.
        glfwInit();

        // Set the options for GLFW.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create a GLFWwindow and make its context current.
        window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwMakeContextCurrent(window_);
        if (window_ == nullptr)
        {
            LOG("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        // Load modern OpenGL mappings.
        if (!gladLoadGL())
        {
            LOG("Failed to initialize OpenGL context");
            glfwTerminate();
            return;
        }
        LOG("Using OpenGL " << GLVersion.major << "." << GLVersion.minor);
    }

    Context::~Context()
    {
        window_ = 0;
        glfwTerminate();
    }
}
