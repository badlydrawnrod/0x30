#pragma once

#include "Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// A batch for drawing quads.
typedef struct {
    GLuint program;             // The shader program to apply for this batch.
    GLuint textureId;           // The texture id.
    GLuint vao;                 // Vertex array object.
    GLuint vertexPosObject;     // Vertex position object.
    GLuint indexObject;         // Vertex index object.
    EmVertexPosTexColour vertices[BATCH_SIZE * VERTICES_PER_QUAD];
    GLushort indices[BATCH_SIZE * INDICES_PER_QUAD];
    GLushort count;             // How many quads are in the batch.
} EmBatch;

void emMakeQuad(const EmTexture* texture, EmRect2v srcRect, EmVec2f dstSize, bool hflip, bool vflip, EmQuad* quad);

EmBatch* emInitializeBatch(GLuint program);
void emTearDownBatch(void);

void emBeginBatch(EmBatch* batch, GLsizei width, GLsizei height);
void emEndBatch(EmBatch* batch);
void emAddQuadToBatch(EmBatch* batch, const EmQuad* quad, const EmPosition* position);
void emAddQuadToBatchEx(EmBatch* batch, const EmQuad* quad, const EmPosition* position, EmRgba4b colour);
