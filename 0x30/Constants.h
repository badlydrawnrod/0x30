#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = 320;
const GLsizei VIRTUAL_HEIGHT = 240;

// Window information.
const GLuint WIDTH = VIRTUAL_WIDTH * 3;
const GLuint HEIGHT = VIRTUAL_HEIGHT * 3;

// Update rates.
const double UPDATE_FPS = 60.0;
const double RENDER_FPS = 60.0;

const char* const TITLE = "The Mysterious 0x30";