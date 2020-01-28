#pragma once

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    GLuint CreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromFile(const char* filename);
}
