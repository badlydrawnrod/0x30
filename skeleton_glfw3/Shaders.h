#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    class Shader
    {
    private:
        GLuint program_{ 0 };

    public:
        Shader();
        ~Shader();

        const GLuint Program() const { return program_; }
    };
}
