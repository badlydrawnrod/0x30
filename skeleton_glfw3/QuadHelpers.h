#pragma once

#include "Batch.h"
#include "Transforms.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    namespace quads
    {
        // Creates a quad that represents an entire texture with top left at (x, y), size (width, height) and colour "colour".
        inline Batch::Quad Create(GLfloat x, GLfloat y, GLfloat width, GLfloat height, Rgba4b colour)
        {
            return Batch::Quad{
                VertexPosTexColour{ { x, y + height }, { 0.0f, 1.0f }, colour },
                VertexPosTexColour{ { x + width, y + height }, { 1.0f, 1.0f }, colour },
                VertexPosTexColour{ { x + width, y }, { 1.0f, 0.0f }, colour },
                VertexPosTexColour{ { x, y }, { 0.0f, 0.0f }, colour }
            };
        }

        // Creates a quad that represents an entire texture with top left at (x, y), and size (width, height).
        inline Batch::Quad Create(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
        {
            const Rgba4b white = { 255, 255, 255, 255 };
            return Create(x, y, width, height, white);
        }

        // Creates a quad that represents a source region of a texture.
        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y, GLfloat srcX, GLfloat srcY, GLfloat srcWidth, GLfloat srcHeight, Rgba4b colour)
        {
            const GLfloat textureWidth = static_cast<GLfloat>(texture.w);
            const GLfloat textureHeight = static_cast<GLfloat>(texture.h);
            const GLfloat u0 = srcX / textureWidth;
            const GLfloat v0 = srcY / textureHeight;
            const GLfloat u1 = (srcX + srcWidth) / textureWidth;
            const GLfloat v1 = (srcY + srcHeight) / textureHeight;

            return Batch::Quad{
                VertexPosTexColour{ { x, y + srcHeight}, { u0, v1 }, colour },
                VertexPosTexColour{ { x + srcWidth, y + srcHeight}, { u1, v1}, colour },
                VertexPosTexColour{ { x + srcWidth, y }, { u1, v0 }, colour },
                VertexPosTexColour{ { x, y }, { u0, v0 }, colour }

            };
        }

        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y, GLfloat srcX, GLfloat srcY, GLfloat srcWidth, GLfloat srcHeight)
        {
            const Rgba4b white = { 255, 255, 255, 255 };
            return Create(texture, x, y, srcX, srcY, srcWidth, srcHeight, white);
        }

        // Creates a quad that represents the entire texture with top left at "position".
        inline Batch::Quad Create(const Texture& texture, Vec2f position)
        {
            return Create(position.x, position.y, static_cast<GLfloat>(texture.w), static_cast<GLfloat>(texture.h));
        }

        // Creates a tinted quad from a textured quad.
        inline Batch::Quad Create(const TexturedQuad& quad, const Position& position, Rgba4b colour)
        {
            // Create the transformed vertices for a textured quad.
            Batch::Quad transformed;
            size_t i = 0;
            for (const VertexPosTex* src = quad.data; src < quad.data + 4; src++, i++)
            {
                Vec2f vertexPos = vec::Transform(src->position, position.centre, position.scale, position.rotation, position.position);
                transformed[i] = VertexPosTexColour{ vertexPos, src->uv, colour };
            }
            return transformed;
        }

        // Creates a quad from a textured quad.
        inline Batch::Quad Create(const TexturedQuad& quad, const Position& position)
        {
            const Rgba4b white = { 255, 255, 255, 255 };
            return Create(quad, position, white);
        }
    }
}
