#pragma once

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
        using QuadPosTexColour = std::array<VertexPosTexColour, 4>;

    public:
        Batch(GLuint program);
        Batch(GLuint program_, size_t size);
        ~Batch();

        void Begin(GLsizei width, GLsizei height);
        void End();

        void Draw();
        void Flush();
        void FlushAsNeeded(GLuint textureId);

        void AddQ(const Texture* texture, const QuadPosTexColour& vertices);
        void AddTexture(const Texture* texture, GLfloat x, GLfloat y, GLfloat width, GLfloat height, Rgba4b colour);
        void AddTexture(const Texture* texture, GLfloat x, GLfloat y, Rgba4b colour);
        void AddTexture(const Texture* texture, GLfloat x, GLfloat y);
        void AddTexture(const Texture* texture, Vec2f position, Rgba4b colour);
        void AddTexture(const Texture* texture, Vec2f position);

        void AddQuad(const Quad* quad, const Position* position, Rgba4b colour);
        void AddQuad(const Quad* quad, const Position* position);

    private:
        void AddVertex(VertexPosTexColour* dst, Vec2f position, Vec2f uv, Rgba4b colour);
        void AddIndices();
    };
}
