#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace je
{
    Quad Quad::MakeQuad(const Texture* texture, Rect2v srcRect, Vec2f dstSize, bool hflip, bool vflip)
    {
        Quad quad;

        // Texture.
        quad.textureId = texture->textureId;

        // Calculate normalised texture coordinates, optionally flipping them horizontally and/or vertically.
        const GLfloat u0 = hflip
            ? (srcRect.position.x + srcRect.size.w) / texture->w
            : srcRect.position.x / texture->w;
        const GLfloat u1 = hflip
            ? srcRect.position.x / texture->w
            : (srcRect.position.x + srcRect.size.w) / texture->w;
        const GLfloat v0 = vflip
            ? (srcRect.position.y + srcRect.size.h) / texture->h
            : srcRect.position.y / texture->h;
        const GLfloat v1 = vflip
            ? srcRect.position.y / texture->h
            : (srcRect.position.y + srcRect.size.h) / texture->h;

        // Half sizes for vertex positions.
        const GLfloat halfWidth = dstSize.w * 0.5f;
        const GLfloat halfHeight = dstSize.h * 0.5f;

        // Bottom left vertex.
        quad.bl.position = { -halfWidth, halfHeight };
        quad.bl.uv = { u0, v1 };

        // Bottom right vertex.
        quad.br.position = { halfWidth, halfHeight };
        quad.br.uv = { u1, v1 };

        // Top right vertex.
        quad.tr.position = { halfWidth, -halfHeight };
        quad.tr.uv = { u1, v0 };

        // Top left vertex.
        quad.tl.position = { -halfWidth, -halfHeight };
        quad.tl.uv = { u0, v0 };

        return quad;
    }
}
