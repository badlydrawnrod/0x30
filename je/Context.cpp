#include "Context.h"

#include "Human.h"
#include "Keyboard.h"
#include "Logger.h"

#include <stdexcept>

namespace je
{
    Context::Context(GLuint width, GLuint height, const GLchar* title)
    {
        // Initialize GLFW.
        glfwInit();

        // Set the options for GLFW.
        const int minOpenGlMajorVersion = 3;
        const int minOpenGlMinorVersion = 3;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, minOpenGlMajorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minOpenGlMinorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create a GLFWwindow and make its context current.
        window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwMakeContextCurrent(window_);
        if (window_ == nullptr)
        {
            LOG("Failed to create GLFW window for OpenGL " << minOpenGlMajorVersion << "." << minOpenGlMinorVersion);
            glfwTerminate();
            throw std::runtime_error("Unable to create GLFW window");
        }
#if !defined(__EMSCRIPTEN__)
        // Load modern OpenGL mappings.
        if (!gladLoadGL())
        {
            LOG("Failed to initialize OpenGL context");
            glfwTerminate();
            throw std::runtime_error("Failed to initialize OpenGL context");
        }

        LOG("Using OpenGL " << GLVersion.major << "." << GLVersion.minor);
#endif
        // Associate the window with the input instance.
        glfwSetWindowUserPointer(window_, Human::Instance());

        // Add the keyboard handler.
        // TODO: could this be part of the input instance, i.e., Human::GetKeyboardHandler().
        glfwSetKeyCallback(window_, GetKeyboardHandler());
    }

    Context::~Context()
    {
        window_ = 0;
        glfwTerminate();
    }
}// namespace je
