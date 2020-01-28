#pragma once

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


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
        VertexPosTexColour vertices_[BATCH_SIZE * VERTICES_PER_QUAD];
        GLushort indices_[BATCH_SIZE * INDICES_PER_QUAD];

    public:
        Batch(GLuint program_);
        ~Batch();

        void Begin(GLsizei width, GLsizei height);
        void End();

        void Draw();
        void Flush();
        void FlushAsNeeded(GLuint textureId);

        void AddQuad(const Quad* quad, const Position* position);
        void AddTintedQuad(const Quad* quad, const Position* position, Rgba4b colour);
    };

    void MakeQuad(const Texture* texture, Rect2v srcRect, Vec2f dstSize, bool hflip, bool vflip, Quad* quad);
}
