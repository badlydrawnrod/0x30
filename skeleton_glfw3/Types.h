#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define BATCH_SIZE 256


// Packed RGBA colour.
typedef union {
    struct {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;
    };
    GLubyte data[4];
} EmRgba4b;

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
} EmVec2f;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat w;
    GLfloat h;
} EmRect4f;

typedef struct {
    EmVec2f position;           // Position.
    EmVec2f size;               // Size.
} EmRect2v;

// Position and texture.
typedef struct {
    EmVec2f position;           // Position.
    EmVec2f uv;                 // Normalised texture coordinates.
} EmVertexPosTex;

// Positional information.
typedef struct {
    EmVec2f position;           // World position.
    EmVec2f centre;             // Centre of rotation relative to world position.
    EmVec2f scale;              // Scaling factor.
    EmVec2f rotation;           // Rotation as cos theta, sin theta.
} EmPosition;

// Position, texture and colour.
typedef struct {
    EmVec2f position;           // Position.
    EmVec2f uv;                 // Normalised texture coordinates.
    EmRgba4b colour;            // Colour.
} EmVertexPosTexColour;

// A texture.
typedef struct {
    GLuint textureId;           // The OpenGL texture id.
    GLsizei w;                  // The width in texels.
    GLsizei h;                  // The height in texels.
} EmTexture;

// A tileset, i.e., a texture with some tile dimensions.
typedef struct {
    EmTexture texture;          // The texture.
    int tileWidth;              // The tile width, in texels.
    int tileHeight;             // The tile height, in texels.
    int widthInTiles;           // The texture's width in tiles.
    int heightInTiles;          // The texture's height in tiles.
} EmTileset;

// A textured quad. These consist of a texture id, then 4 vertices giving the positions and normalized texture
// coordinates of the quad's corners.
typedef struct {
    GLuint textureId;           // The OpenGL texture id.
    union {
        struct {
            EmVertexPosTex bl;  // Bottom left.
            EmVertexPosTex br;  // Bottom right.
            EmVertexPosTex tr;  // Top right.
            EmVertexPosTex tl;  // Top left.
        };
        EmVertexPosTex data[4]; // vertex positions and texture region offsets (in order BL, BR, TR, TL).
    };
} EmQuad;
