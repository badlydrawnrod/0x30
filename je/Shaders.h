#pragma once
#include "Platform.h"

namespace je
{
    class Shader
    {
    private:
        GLuint program_{0};

    public:
        Shader();
        ~Shader();

        GLuint Program() const
        {
            return program_;
        }
    };
} // namespace je
