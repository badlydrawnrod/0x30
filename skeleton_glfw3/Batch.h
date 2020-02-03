#pragma once

#include "Transforms.h"
#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <vector>


namespace je
{
    // A batch for drawing quads.
    class Batch
    {
    private:
        GLuint program_{ 0 };             // The shader program to apply for this batch.
        GLuint textureId_{ 0 };           // The texture id.
        GLuint vao_{ 0 };                 // Vertex array object.
        GLuint vertexPosObject_{ 0 };     // Vertex position object.
        GLuint indexObject_{ 0 };         // Vertex index object.
        GLushort count_{ 0 };             // How many quads are in the batch.
        size_t batchSize_;                // The maximum number of quads in the batch.
        std::vector <VertexPosTexColour> vertices_;
        std::vector <GLushort> indices_;

    public:
        using Quad = std::array<VertexPosTexColour, 4>;
        struct TQuad
        {
            GLuint textureId_;
            Quad quad_;
        };

    public:
        Batch(GLuint program);
        Batch(GLuint program_, size_t size);
        ~Batch();

        void Begin(GLsizei width, GLsizei height);
        void End();

        void Draw();
        void Flush();
        void FlushAsNeeded(GLuint textureId);

        void AddVertices(GLuint textureId, const Quad& vertices);
        void AddVertices(const TQuad& vertices);
    };

    inline void Batch::AddVertices(const TQuad& vertices)
    {
        AddVertices(vertices.textureId_, vertices.quad_);
    }
}
