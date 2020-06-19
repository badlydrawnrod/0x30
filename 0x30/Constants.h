#pragma once

#include "je/Platform.h"

// TODO: Should these really be OpenGL types?

// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = 320;
const GLsizei VIRTUAL_HEIGHT = 240;

// Window information.
const GLuint WIDTH = VIRTUAL_WIDTH * 3;
const GLuint HEIGHT = VIRTUAL_HEIGHT * 3;

// Update rates.
const double UPDATE_FPS = 60.0;
const double RENDER_FPS = 60.0;

const char TITLE[] = "Happy XLVIII";
