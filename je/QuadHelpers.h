#pragma once

#include "Batch.h"
#include "Transforms.h"

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace je
{
    namespace quads
    {
        // Creates a quad that represents an entire texture with top left at (x, y), size (width, height) and colour "colour".
        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y, GLfloat width, GLfloat height, Rgba4b colour)
        {
            return Batch::Quad{
                    texture.textureId,
                    Batch::Vertices{
                            VertexPosTexColour{{x, y + height}, {0.0f, 1.0f}, colour},
                            VertexPosTexColour{{x + width, y + height}, {1.0f, 1.0f}, colour},
                            VertexPosTexColour{{x + width, y}, {1.0f, 0.0f}, colour},
                            VertexPosTexColour{{x, y}, {0.0f, 0.0f}, colour}}};
        }

        // Creates a quad that represents an entire texture with top left at (x, y), and size (width, height).
        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y, GLfloat width, GLfloat height)
        {
            const Rgba4b white = {255, 255, 255, 255};
            return Create(texture, x, y, width, height, white);
        }

        // Creates a quad that represents the entire texture with top left at (x, y).
        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y)
        {
            return Create(texture, x, y, static_cast<GLfloat>(texture.w), static_cast<GLfloat>(texture.h));
        }

        // Creates a quad that represents the entire texture with top left at "position".
        inline Batch::Quad Create(const Texture& texture, Vec2f position)
        {
            return Create(texture, position.x, position.y, static_cast<GLfloat>(texture.w), static_cast<GLfloat>(texture.h));
        }

        // Creates a quad that represents an entire texture region with top left at (x, y), and size to (width, height).
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y, GLfloat width, GLfloat height, Rgba4b colour)
        {
            const GLfloat textureWidth = static_cast<GLfloat>(region.texture.w);
            const GLfloat textureHeight = static_cast<GLfloat>(region.texture.h);
            const GLfloat u0 = region.x / textureWidth;
            const GLfloat v0 = region.y / textureHeight;
            const GLfloat u1 = (region.x + region.w) / textureWidth;
            const GLfloat v1 = (region.y + region.h) / textureHeight;

            return Batch::Quad{
                    region.texture.textureId,
                    Batch::Vertices{
                            VertexPosTexColour{{x, y + height}, {u0, v1}, colour},
                            VertexPosTexColour{{x + width, y + height}, {u1, v1}, colour},
                            VertexPosTexColour{{x + width, y}, {u1, v0}, colour},
                            VertexPosTexColour{{x, y}, {u0, v0}, colour}}};
        }

        // Creates a quad that represents an entire texture region with top left at (x, y), and size to (width, height).
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y, GLfloat width, GLfloat height)
        {
            const Rgba4b white = {255, 255, 255, 255};
            return Create(region, x, y, width, height, white);
        }

        // Creates a quad that represents a texture region with top left at (x, y).
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y, Rgba4b colour)
        {
            const GLfloat textureWidth = static_cast<GLfloat>(region.texture.w);
            const GLfloat textureHeight = static_cast<GLfloat>(region.texture.h);
            const GLfloat u0 = region.x / textureWidth;
            const GLfloat v0 = region.y / textureHeight;
            const GLfloat u1 = (region.x + region.w) / textureWidth;
            const GLfloat v1 = (region.y + region.h) / textureHeight;

            return Batch::Quad{
                    region.texture.textureId,
                    Batch::Vertices{
                            VertexPosTexColour{{x, y + region.h}, {u0, v1}, colour},
                            VertexPosTexColour{{x + region.w, y + region.h}, {u1, v1}, colour},
                            VertexPosTexColour{{x + region.w, y}, {u1, v0}, colour},
                            VertexPosTexColour{{x, y}, {u0, v0}, colour}}};
        }

        // Creates a quad that represents a texture region with top left at (x, y).
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y)
        {
            const Rgba4b white = {255, 255, 255, 255};
            return Create(region, x, y, white);
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
                    texture.textureId,
                    Batch::Vertices{
                            VertexPosTexColour{{x, y + srcHeight}, {u0, v1}, colour},
                            VertexPosTexColour{{x + srcWidth, y + srcHeight}, {u1, v1}, colour},
                            VertexPosTexColour{{x + srcWidth, y}, {u1, v0}, colour},
                            VertexPosTexColour{{x, y}, {u0, v0}, colour}}};
        }

        inline Batch::Quad Create(const Texture& texture, GLfloat x, GLfloat y, GLfloat srcX, GLfloat srcY, GLfloat srcWidth, GLfloat srcHeight)
        {
            const Rgba4b white = {255, 255, 255, 255};
            return Create(texture, x, y, srcX, srcY, srcWidth, srcHeight, white);
        }

        // Creates a quad that represents a source region of a texture region.
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y, GLfloat srcX, GLfloat srcY, GLfloat srcWidth, GLfloat srcHeight, Rgba4b colour)
        {
            const GLfloat textureX = region.x + srcX;
            const GLfloat textureY = region.y + srcY;
            return Create(region.texture, x, y, textureX, textureY, srcWidth, srcHeight, colour);
        }

        // Creates a quad that represents a source region of a texture region.
        inline Batch::Quad Create(const TextureRegion& region, GLfloat x, GLfloat y, GLfloat srcX, GLfloat srcY, GLfloat srcWidth, GLfloat srcHeight)
        {
            const Rgba4b white = {255, 255, 255, 255};
            return Create(region, x, y, srcX, srcY, srcWidth, srcHeight, white);
        }
    }// namespace quads
}// namespace je
