#pragma once

#include "Platform.h"

namespace je
{
    // Packed RGBA colour.
    struct Rgba4b
    {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;
    };

    // A 2 element float vector.
    struct Vec2f
    {
        GLfloat x;
        GLfloat y;
    };

    // A rectangle.
    struct Rect4f
    {
        GLfloat x;
        GLfloat y;
        GLfloat w;
        GLfloat h;
    };

    // A rectangle based on position and size.
    struct Rect2v
    {
        Vec2f position; // Position.
        Vec2f size;     // Size.
    };

    // Position and texture.
    struct VertexPosTex
    {
        Vec2f position; // Position.
        Vec2f uv;       // Normalised texture coordinates.
    };

    // Positional information.
    struct Position
    {
        Vec2f position; // World position.
        Vec2f centre;   // Centre of rotation relative to world position.
        Vec2f scale;    // Scaling factor.
        Vec2f rotation; // Rotation as cos theta, sin theta.
    };

    // Position, texture and colour.
    struct VertexPosTexColour
    {
        Vec2f position; // Position.
        Vec2f uv;       // Normalised texture coordinates.
        Rgba4b colour;  // Colour.
    };

    // A texture.
    struct Texture
    {
        GLuint textureId; // The OpenGL texture id.
        GLsizei w;        // The width in texels.
        GLsizei h;        // The height in texels.
    };

    // A rectangular region of a texture.
    struct TextureRegion
    {
        Texture texture; // The texture associated with the region.
        GLfloat x;       // The x coordinate of the top left of the region, in texels.
        GLfloat y;       // The y coordinate of the top left of the region, in texels.
        GLfloat w;       // The width in texels.
        GLfloat h;       // The height in texels.
    };

    // A tileset, i.e., a texture with some tile dimensions.
    struct Tileset
    {
        Texture texture;   // The texture.
        int tileWidth;     // The tile width, in texels.
        int tileHeight;    // The tile height, in texels.
        int widthInTiles;  // The texture's width in tiles.
        int heightInTiles; // The texture's height in tiles.
    };
} // namespace je
