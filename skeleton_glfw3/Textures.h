#pragma once

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


GLuint emCreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height);
EmTexture emLoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height);
EmTexture emLoadTextureFromFile(const char* filename);
