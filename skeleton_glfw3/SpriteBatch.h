#pragma once

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace je
{
    // A batch for drawing quads.
    typedef struct {
        GLuint program;             // The shader program to apply for this batch.
        GLuint textureId;           // The texture id.
        GLuint vao;                 // Vertex array object.
        GLuint vertexPosObject;     // Vertex position object.
        GLuint indexObject;         // Vertex index object.
        VertexPosTexColour vertices[BATCH_SIZE * VERTICES_PER_QUAD];
        GLushort indices[BATCH_SIZE * INDICES_PER_QUAD];
        GLushort count;             // How many quads are in the batch.
    } Batch;

    void MakeQuad(const Texture* texture, Rect2v srcRect, Vec2f dstSize, bool hflip, bool vflip, Quad* quad);

    Batch* InitializeBatch(GLuint program);
    void TearDownBatch(void);

    void BeginBatch(Batch* batch, GLsizei width, GLsizei height);
    void EndBatch(Batch* batch);
    void AddQuadToBatch(Batch* batch, const Quad* quad, const Position* position);
    void AddQuadToBatchEx(Batch* batch, const Quad* quad, const Position* position, Rgba4b colour);
}
