#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define BATCH_SIZE 256

namespace je
{
    // Packed RGBA colour.
    typedef union {
        struct {
            GLubyte r;
            GLubyte g;
            GLubyte b;
            GLubyte a;
        };
        GLubyte data[4];
    } Rgba4b;

    // A 2 element float vector, with aliases for common usages.
    typedef union {
        struct {
            GLfloat x;
            GLfloat y;
        };
        struct {
            GLfloat u;
            GLfloat v;
        };
        struct {
            GLfloat w;
            GLfloat h;
        };
        struct {
            GLfloat cos;
            GLfloat sin;
        };
        GLfloat data[2];
    } Vec2f;

    typedef struct {
        GLfloat x;
        GLfloat y;
        GLfloat w;
        GLfloat h;
    } Rect4f;

    typedef struct {
        Vec2f position;             // Position.
        Vec2f size;                 // Size.
    } Rect2v;

    // Position and texture.
    typedef struct {
        Vec2f position;             // Position.
        Vec2f uv;                   // Normalised texture coordinates.
    } VertexPosTex;

    // Positional information.
    typedef struct {
        Vec2f position;             // World position.
        Vec2f centre;               // Centre of rotation relative to world position.
        Vec2f scale;                // Scaling factor.
        Vec2f rotation;             // Rotation as cos theta, sin theta.
    } Position;

    // Position, texture and colour.
    typedef struct {
        Vec2f position;             // Position.
        Vec2f uv;                   // Normalised texture coordinates.
        Rgba4b colour;              // Colour.
    } VertexPosTexColour;

    // A texture.
    typedef struct {
        GLuint textureId;           // The OpenGL texture id.
        GLsizei w;                  // The width in texels.
        GLsizei h;                  // The height in texels.
    } Texture;

    // A tileset, i.e., a texture with some tile dimensions.
    typedef struct {
        Texture texture;            // The texture.
        int tileWidth;              // The tile width, in texels.
        int tileHeight;             // The tile height, in texels.
        int widthInTiles;           // The texture's width in tiles.
        int heightInTiles;          // The texture's height in tiles.
    } Tileset;

    // A textured quad. These consist of a texture id, then 4 vertices giving the positions and normalized texture
    // coordinates of the quad's corners.
    typedef struct {
        GLuint textureId;           // The OpenGL texture id.
        union {
            struct {
                VertexPosTex bl;    // Bottom left.
                VertexPosTex br;    // Bottom right.
                VertexPosTex tr;    // Top right.
                VertexPosTex tl;    // Top left.
            };
            VertexPosTex data[4];   // Vertex positions and texture region offsets (in order BL, BR, TR, TL).
        };
    } Quad;
}
