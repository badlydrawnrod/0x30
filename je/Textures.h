#pragma once

#include "Types.h"

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace je
{
    GLuint CreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromFile(const char* filename);
}// namespace je
