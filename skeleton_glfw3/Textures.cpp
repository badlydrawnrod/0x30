#include "Textures.h"

#include "Logger.h"
#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_image.h>


GLuint emCreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height)
{
    GLuint textureId;

    // Generate a texture object.
    glGenTextures(1, &textureId);

    // Bind the texture object.
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Use tightly packed data.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set the filtering mode.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load the texture into OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}


SDL_Surface* emLoadImage(const char* filename)
{
    SDL_Surface* image = IMG_Load(filename);
    if (!image)
    {
        LOG("Failed to load surface");
        return nullptr;
    }

    return image;
}


EmTexture emLoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height)
{
    GLuint texture1 = emCreateTextureFromPixels(pixels, width, height);
    return EmTexture{ texture1, width, height };
}


EmTexture emLoadTextureFromFile(const char* filename)
{
    LOG("Loading texture from " << filename);
    SDL_Surface* image = emLoadImage(filename);
    EmTexture texture = emLoadTextureFromMemory(image->pixels, image->w, image->h);
    SDL_FreeSurface(image);
    return texture;
}
