#include "Batch.h"

#include "Platform.h"
#include "Types.h"

namespace je
{
    static constexpr size_t DEFAULT_BATCH_SIZE = 256;
    static constexpr size_t VERTICES_PER_QUAD = 4;
    static constexpr size_t INDICES_PER_QUAD = 6;

    Batch::Batch(GLuint program)
        : Batch(program, DEFAULT_BATCH_SIZE)
    {
    }

    Batch::Batch(GLuint program, size_t size)
        : program_(program),
          batchSize_(size),
          vertices_(size * VERTICES_PER_QUAD),
          indices_(size * INDICES_PER_QUAD)
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
        GLfloat resolution[2] = {static_cast<GLfloat>(width), static_cast<GLfloat>(height)};
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
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexColour), &((VertexPosTexColour*)0)->position);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexColour), &((VertexPosTexColour*)0)->uv);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexPosTexColour), &((VertexPosTexColour*)0)->colour);

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
        glBufferData(GL_ARRAY_BUFFER, VERTICES_PER_QUAD * sizeof(VertexPosTexColour) * count_, vertices_.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES_PER_QUAD * sizeof(indices_[0]) * count_, indices_.data(), GL_STATIC_DRAW);

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
        if (count_ == batchSize_)
        {
            Flush();
        }
    }

    void Batch::AddVertices(GLuint textureId, const Vertices& vertices)
    {
        FlushAsNeeded(textureId);

        // Add the vertices to the batch.
        VertexPosTexColour* vertex = &vertices_[count_ * VERTICES_PER_QUAD];
        *vertex++ = vertices[0];
        *vertex++ = vertices[1];
        *vertex++ = vertices[2];
        *vertex++ = vertices[3];

        // Add the indices to the batch.
        const GLushort ofs = count_ * VERTICES_PER_QUAD;
        GLushort* index = &indices_[count_ * INDICES_PER_QUAD];
        *index++ = ofs + static_cast<GLushort>(0);
        *index++ = ofs + static_cast<GLushort>(1);
        *index++ = ofs + static_cast<GLushort>(2);
        *index++ = ofs + static_cast<GLushort>(2);
        *index++ = ofs + static_cast<GLushort>(3);
        *index = ofs + static_cast<GLushort>(0);

        count_++;
    }
} // namespace je
