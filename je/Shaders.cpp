#include "Shaders.h"

#include "Logger.h"

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>


namespace je
{
    Shader::Shader()
    {
        // Vertex shader. It knows about 2D positions and texture coordinates, but knows nothing of rotation or scaling as
        // that is assumed to have been done already, e.g., by a sprite batch.
        static const GLchar* vertexShaderCode =
            "#version 300 es                                        \n"
            "uniform vec2 u_resolution;                             \n"
            "layout(location = 0) in vec2 a_position;               \n"
            "layout(location = 1) in vec2 a_texCoord;               \n"
            "layout(location = 2) in vec4 a_color;                  \n"
            "out vec2 v_texCoord;                                   \n"
            "out vec4 v_color;                                      \n"
            "void main()                                            \n"
            "{                                                      \n"
            "   vec2 pos = a_position;                              \n"
            "   vec2 clipSpace = ((pos / u_resolution) * 2.0) - 1.0;\n"
            "   gl_Position = vec4(clipSpace * vec2(1, -1), 0, 1);  \n"
            "   v_texCoord = a_texCoord;                            \n"
            "   v_color = a_color;                                  \n"
            "}";

        // Fragment shader. It discards any pixel whose alpha value is < 0.1.
        // See https://learnopengl.com/Advanced-OpenGL/Blending
        static const GLchar* fragmentShaderCode =
            "#version 300 es                                                \n"
            "precision mediump float;                                       \n"
            "in vec2 v_texCoord;                                            \n"
            "in vec4 v_color;                                               \n"
            "layout(location = 0) out vec4 outColor;                        \n"
            "uniform sampler2D s_texture;                                   \n"
            "void main()                                                    \n"
            "{                                                              \n"
            "  vec4 texColour = v_color * texture(s_texture, v_texCoord);   \n"
            "  if (texColour.a < 0.1)                                       \n"
            "    discard;                                                   \n"
            "  outColor = texColour;                                        \n"
            "}";

        // Create and compile the vertex shader.
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if (!glIsShader(vertexShader))
        {
            LOG("Failed to create vertex shader");
            return;
        }
        glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
        glCompileShader(vertexShader);
        {
            GLint params = 0;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &params);
            if (params != GL_TRUE)
            {
                LOG("Failed to compile vertex shader");
                return;
            }
        }

        // Create and compile the fragment shader.
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!glIsShader(fragmentShader))
        {
            LOG("Failed to create fragment shader");
            return;
        }
        glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
        glCompileShader(fragmentShader);
        {
            GLint params = 0;
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &params);
            if (params != GL_TRUE)
            {
                LOG("Failed to compile fragment shader");
                return;
            }
        }

        // Create the program, attach the shaders to it, and link it
        program_ = glCreateProgram();
        if (!glIsProgram(program_))
        {
            LOG("Failed to create shader program");
        }

        glAttachShader(program_, vertexShader);
        glAttachShader(program_, fragmentShader);
        glLinkProgram(program_);

        // Delete the shaders as the program owns them now.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader()
    {
        if (glIsProgram(program_))
        {
            glDeleteProgram(program_);
            program_ = 0;
        }
    }
}
