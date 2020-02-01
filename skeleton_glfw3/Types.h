#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    // Packed RGBA colour.
    union Rgba4b
    {
        struct
        {
            GLubyte r;
            GLubyte g;
            GLubyte b;
            GLubyte a;
        };
        GLubyte data[4];
    };

    // A 2 element float vector, with aliases for common usages.
    union Vec2f
    {
        struct
        {
            GLfloat x;
            GLfloat y;
        };
        struct
        {
            GLfloat u;
            GLfloat v;
        };
        struct
        {
            GLfloat w;
            GLfloat h;
        };
        struct
        {
            GLfloat cos;
            GLfloat sin;
        };
        GLfloat data[2];
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
        Vec2f position;             // Position.
        Vec2f size;                 // Size.
    };

    // Position and texture.
    struct VertexPosTex
    {
        Vec2f position;             // Position.
        Vec2f uv;                   // Normalised texture coordinates.
    };

    // Positional information.
    struct Position
    {
        Vec2f position;             // World position.
        Vec2f centre;               // Centre of rotation relative to world position.
        Vec2f scale;                // Scaling factor.
        Vec2f rotation;             // Rotation as cos theta, sin theta.
    };

    // Position, texture and colour.
    struct VertexPosTexColour {
        Vec2f position;             // Position.
        Vec2f uv;                   // Normalised texture coordinates.
        Rgba4b colour;              // Colour.
    };

    // A texture.
    struct Texture
    {
        GLuint textureId;           // The OpenGL texture id.
        GLsizei w;                  // The width in texels.
        GLsizei h;                  // The height in texels.
    };

    // A tileset, i.e., a texture with some tile dimensions.
    struct Tileset
    {
        Texture texture;            // The texture.
        int tileWidth;              // The tile width, in texels.
        int tileHeight;             // The tile height, in texels.
        int widthInTiles;           // The texture's width in tiles.
        int heightInTiles;          // The texture's height in tiles.
    };

    // A textured quad. These consist of a texture id, then 4 vertices giving the positions and normalized texture
    // coordinates of the quad's corners.
    struct TexturedQuad
    {
        GLuint textureId;           // The OpenGL texture id.
        union
        {
            struct
            {
                VertexPosTex bl;    // Bottom left.
                VertexPosTex br;    // Bottom right.
                VertexPosTex tr;    // Top right.
                VertexPosTex tl;    // Top left.
            };
            VertexPosTex data[4];   // Vertex positions and texture region offsets (in order BL, BR, TR, TL).
        };

        static TexturedQuad Create(const Texture* texture, Rect2v srcRect, Vec2f dstSize, bool hflip = false, bool vflip = false);
    };
}
