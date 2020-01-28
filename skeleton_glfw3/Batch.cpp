#include "Batch.h"

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    void MakeQuad(const Texture* texture, Rect2v srcRect, Vec2f dstSize, bool hflip, bool vflip, Quad* quad)
    {
        // Texture.
        quad->textureId = texture->textureId;

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
        quad->bl.position = { -halfWidth, -halfHeight };
        quad->bl.uv = { u0, v0 };

        // Bottom right vertex.
        quad->br.position = { halfWidth, -halfHeight };
        quad->br.uv = { u1, v0 };

        // Top right vertex.
        quad->tr.position = { halfWidth, halfHeight };
        quad->tr.uv = { u1, v1 };

        // Top left vertex.
        quad->tl.position = { -halfWidth, halfHeight };
        quad->tl.uv = { u0, v1 };
    }

    Batch::Batch(GLuint program_) : program_(program_)
    {
    }

    Batch::~Batch()
    {
        glBindVertexArray(0);
        glUseProgram(0);
        program_ = 0;
        if (vao_ != 0)
        {
            glDeleteVertexArrays(1, &vao_);
        }
    }

    void Batch::Begin(GLsizei width, GLsizei height)
    {
        // Use the program object when drawing.
        glUseProgram(program_);

        // Set the resolution uniform.
        GLint resolutionLocation = glGetUniformLocation(program_, "u_resolution");
        GLfloat resolution[2] = { static_cast<GLfloat>(width), static_cast<GLfloat>(height) };
        glUniform2fv(resolutionLocation, 1, resolution);

        // Set the sampler uniform.
        GLint samplerLocation = glGetUniformLocation(program_, "s_texture");
        glUniform1i(samplerLocation, 0);

        // Create a VAO if required and save our vertex array state in it.
        if (vao_ == 0)
        {
            // Create a vertex array object and bind to it.
            glGenVertexArrays(1, &vao_);
            glBindVertexArray(vao_);

            // Create vertex buffer objects and bind to them.
            glGenBuffers(1, &vertexPosObject_);
            glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject_);
            glGenBuffers(1, &indexObject_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObject_);

            // Tell OpenGL where to find the vertex array, texture array, and the colour array.
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexColour),
                &((VertexPosTexColour*)0)->position);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexColour),
                &((VertexPosTexColour*)0)->uv);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexPosTexColour),
                &((VertexPosTexColour*)0)->colour);

            glBindVertexArray(0);
        }

        // Set the vertex array state to what is in our VAO.
        glBindVertexArray(vao_);

        // Start the batch empty, with no active texture.
        count_ = 0;
        textureId_ = 0;

        // Enable and configure blending.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Batch::Draw()
    {
        if (count_ == 0)
        {
            return;
        }

        // Send the vertices and indices.
        glBufferData(GL_ARRAY_BUFFER, VERTICES_PER_QUAD * sizeof(VertexPosTexColour) * count_, vertices_,
            GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES_PER_QUAD * sizeof(indices_[0]) * count_, indices_,
            GL_STATIC_DRAW);

        // Draw a lovely bunch of triangles.
        glDrawElements(GL_TRIANGLES, count_ * INDICES_PER_QUAD, GL_UNSIGNED_SHORT, 0);
    }

    void Batch::Flush()
    {
        Draw();
        count_ = 0;
    }

    void Batch::End()
    {
        Flush();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Batch::FlushAsNeeded(GLuint textureId)
    {
        // If the texture has changed then flush the batch and activate the new texture.
        if (textureId_ != textureId)
        {
            if (textureId_ != 0)
            {
                Flush();
            }
            textureId_ = textureId;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId_);
        }

        // If the batch is full then flush it.
        if (count_ == BATCH_SIZE)
        {
            Flush();
        }
    }

    void Batch::AddTintedQuad(const Quad* quad, const Position* position, Rgba4b colour)
    {
        FlushAsNeeded(quad->textureId);

        // Append the quad's vertices to the batch, rotating, scaling and translating as we go.
        const VertexPosTex* src = quad->data;
        VertexPosTexColour* dst = vertices_ + count_ * VERTICES_PER_QUAD;
        for (int i = 0; i < 4; i++)
        {
            // Load from src, adjust for the centre of rotation, then scale. Here we assume that a quad is drawn around
            // its origin by default, and that internal quad coordinates are x right, y up.
            const GLfloat x = (src->position.x - position->centre.x) * position->scale.x;
            const GLfloat y = (src->position.y - position->centre.y) * position->scale.y;

            // Rotate it around its centre.
            const GLfloat rotX = x * position->rotation.cos - y * position->rotation.sin;
            const GLfloat rotY = x * position->rotation.sin + y * position->rotation.cos;

            // Translate it into position and store to dst.
            dst->position.x = rotX + position->position.x;
            dst->position.y = rotY + position->position.y;

            // Texture coords.
            dst->uv = src->uv;

            // Colour.
            dst->colour = colour;

            // Next vertex.
            dst++;
            src++;
        }

        // Add the indices of the quad to the batch.
        const GLushort ofs = count_ * (GLushort)VERTICES_PER_QUAD;
        GLushort* p = indices_ + count_ * INDICES_PER_QUAD;
        *p++ = ofs + (GLushort)0;
        *p++ = ofs + (GLushort)1;
        *p++ = ofs + (GLushort)2;
        *p++ = ofs + (GLushort)2;
        *p++ = ofs + (GLushort)3;
        *p = ofs + (GLushort)0;

        count_++;
    }

    void Batch::AddQuad(const Quad* quad, const Position* position)
    {
        Rgba4b white = { 255, 255, 255, 255 };
        AddTintedQuad(quad, position, white);
    }
}