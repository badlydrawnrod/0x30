#pragma once

#include "Platform.h"
#include "Types.h"

namespace je
{
    GLuint CreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height);
    Texture LoadTextureFromFile(const char* filename);
} // namespace je
