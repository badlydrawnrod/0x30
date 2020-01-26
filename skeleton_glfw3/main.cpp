#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_image.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>


// --- General types ---------------------------------------------------------------------------------------------------


#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define BATCH_SIZE 256


// Packed RGBA colour.
typedef union {
    struct {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;
    };
    GLubyte data[4];
} EmRgba4b;

// A 2 element float vector, with aliases for common usages.
typedef union {
    struct {
        GLfloat x;
        GLfloat y;
    };
    struct {
        GLfloat u;
        GLfloat v;
    };
    struct {
        GLfloat w;
        GLfloat h;
    };
    struct {
        GLfloat cos;
        GLfloat sin;
    };
    GLfloat data[2];
} EmVec2f;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat w;
    GLfloat h;
} EmRect4f;

typedef struct {
    EmVec2f position;           // Position.
    EmVec2f size;               // Size.
} EmRect2v;

// Position and texture.
typedef struct {
    EmVec2f position;           // Position.
    EmVec2f uv;                 // Normalised texture coordinates.
} EmVertexPosTex;

// Positional information.
typedef struct {
    EmVec2f position;           // World position.
    EmVec2f centre;             // Centre of rotation relative to world position.
    EmVec2f scale;              // Scaling factor.
    EmVec2f rotation;           // Rotation as cos theta, sin theta.
} EmPosition;

// Position, texture and colour.
typedef struct {
    EmVec2f position;           // Position.
    EmVec2f uv;                 // Normalised texture coordinates.
    EmRgba4b colour;            // Colour.
} EmVertexPosTexColour;

// A texture.
typedef struct {
    GLuint textureId;           // The OpenGL texture id.
    GLsizei w;                  // The width in texels.
    GLsizei h;                  // The height in texels.
} EmTexture;

// A tileset, i.e., a texture with some tile dimensions.
typedef struct {
    EmTexture texture;          // The texture.
    int tileWidth;              // The tile width, in texels.
    int tileHeight;             // The tile height, in texels.
    int widthInTiles;           // The texture's width in tiles.
    int heightInTiles;          // The texture's height in tiles.
} EmTileset;

// A textured quad. These consist of a texture id, then 4 vertices giving the positions and normalized texture
// coordinates of the quad's corners.
typedef struct {
    GLuint textureId;           // The OpenGL texture id.
    union {
        struct {
            EmVertexPosTex bl;  // Bottom left.
            EmVertexPosTex br;  // Bottom right.
            EmVertexPosTex tr;  // Top right.
            EmVertexPosTex tl;  // Top left.
        };
        EmVertexPosTex data[4]; // vertex positions and texture region offsets (in order BL, BR, TR, TL).
    };
} EmQuad;


// --- Textures --------------------------------------------------------------------------------------------------------


GLuint emCreateTextureFromPixels(GLvoid* pixels, GLsizei width, GLsizei height)
{
    GLuint textureId;

    // Generate a texture object.
    glGenTextures(1, &textureId);

    // Bind the texture object.
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Use tightly packed data.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set the filtering mode.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load the texture into OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}


SDL_Surface* emLoadImage(const char* filename)
{
    SDL_Surface* image = IMG_Load(filename);
    if (!image)
    {
        LOG("Failed to load surface");
        return nullptr;
    }

    return image;
}


EmTexture emLoadTextureFromMemory(GLvoid* pixels, GLsizei width, GLsizei height)
{
    GLuint texture1 = emCreateTextureFromPixels(pixels, width, height);
    return EmTexture{ texture1, width, height };
}


EmTexture emLoadTextureFromFile(const char* filename)
{
    LOG("Loading texture from " << filename);
    SDL_Surface* image = emLoadImage(filename);
    EmTexture texture = emLoadTextureFromMemory(image->pixels, image->w, image->h);
    SDL_FreeSurface(image);
    return texture;
}


// --- Batch -----------------------------------------------------------------------------------------------------------


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


EmBatch emBatch = { 0 }; // The batch.


void emMakeQuad(const EmTexture* texture, EmRect2v srcRect, EmVec2f dstSize, bool hflip, bool vflip, EmQuad* quad)
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

EmBatch* emInitializeBatch(GLuint program)
{
    EmBatch* batch = &emBatch;
    batch->program = program;
    return batch;
}

void emTearDownBatch(void)
{
    EmBatch* batch = &emBatch;
    glBindVertexArray(0);
    glUseProgram(0);
    batch->program = 0;
    if (batch->vao != 0)
    {
        glDeleteVertexArrays(1, &batch->vao);
    }
}

void emBeginBatch(EmBatch* batch, GLsizei width, GLsizei height)
{
    // Use the program object when drawing.
    glUseProgram(batch->program);

    // Set the resolution uniform.
    GLint resolutionLocation = glGetUniformLocation(batch->program, "u_resolution");
    GLfloat resolution[2] = { static_cast<GLfloat>(width), static_cast<GLfloat>(height) };
    glUniform2fv(resolutionLocation, 1, resolution);

    // Set the sampler uniform.
    GLint samplerLocation = glGetUniformLocation(batch->program, "s_texture");
    glUniform1i(samplerLocation, 0);

    // Create a VAO if required and save our vertex array state in it.
    if (batch->vao == 0)
    {
        // Create a vertex array object and bind to it.
        glGenVertexArrays(1, &batch->vao);
        glBindVertexArray(batch->vao);

        // Create vertex buffer objects and bind to them.
        glGenBuffers(1, &batch->vertexPosObject);
        glBindBuffer(GL_ARRAY_BUFFER, batch->vertexPosObject);
        glGenBuffers(1, &batch->indexObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->indexObject);

        // Tell OpenGL where to find the vertex array, texture array, and the colour array.
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EmVertexPosTexColour),
            &((EmVertexPosTexColour*)0)->position);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EmVertexPosTexColour),
            &((EmVertexPosTexColour*)0)->uv);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(EmVertexPosTexColour),
            &((EmVertexPosTexColour*)0)->colour);

        glBindVertexArray(0);
    }

    // Set the vertex array state to what is in our VAO.
    glBindVertexArray(batch->vao);

    // Start the batch empty, with no active texture.
    batch->count = 0;
    batch->textureId = 0;

    // Enable and configure blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void emDrawBatch(EmBatch* batch)
{
    if (batch->count == 0)
    {
        return;
    }

    // Send the vertices and indices.
    glBufferData(GL_ARRAY_BUFFER, VERTICES_PER_QUAD * sizeof(EmVertexPosTexColour) * batch->count, batch->vertices,
        GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES_PER_QUAD * sizeof(batch->indices[0]) * batch->count, batch->indices,
        GL_STATIC_DRAW);

    // Draw a lovely bunch of triangles.
    glDrawElements(GL_TRIANGLES, batch->count * INDICES_PER_QUAD, GL_UNSIGNED_SHORT, 0);
}

void emFlushBatch(EmBatch* batch)
{
    emDrawBatch(batch);
    batch->count = 0;
}

void emEndBatch(EmBatch* batch)
{
    emFlushBatch(batch);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}

void emFlushBatchAsNeeded(EmBatch* batch, GLuint textureId)
{
    // If the texture has changed then flush the batch and activate the new texture.
    if (textureId != batch->textureId)
    {
        if (batch->textureId != 0)
        {
            emFlushBatch(batch);
        }
        batch->textureId = textureId;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batch->textureId);
    }

    // If the batch is full then flush it.
    if (batch->count == BATCH_SIZE)
    {
        emFlushBatch(batch);
    }
}

void emAddQuadToBatchEx(EmBatch* batch, const EmQuad* quad, const EmPosition* position, EmRgba4b colour)
{
    emFlushBatchAsNeeded(batch, quad->textureId);

    // Append the quad's vertices to the batch, rotating, scaling and translating as we go.
    const EmVertexPosTex* src = quad->data;
    EmVertexPosTexColour* dst = batch->vertices + batch->count * VERTICES_PER_QUAD;
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
    const GLushort ofs = batch->count * (GLushort)VERTICES_PER_QUAD;
    GLushort* p = batch->indices + batch->count * INDICES_PER_QUAD;
    *p++ = ofs + (GLushort)0;
    *p++ = ofs + (GLushort)1;
    *p++ = ofs + (GLushort)2;
    *p++ = ofs + (GLushort)2;
    *p++ = ofs + (GLushort)3;
    *p = ofs + (GLushort)0;

    batch->count++;
}

void emAddQuadToBatch(EmBatch* batch, const EmQuad* quad, const EmPosition* position)
{
    EmRgba4b white = { 255, 255, 255, 255 };
    emAddQuadToBatchEx(batch, quad, position, white);
}



// --- Shaders ---------------------------------------------------------------------------------------------------------


GLuint program;


GLuint InitializeShaders()
{
    // Vertex shader. It knows about 2D positions and texture coordinates, but knows nothing of rotation or scaling as
    // that is assumed to have been done already, e.g., by a sprite batch.
    static const GLchar* vertexShaderCode =
        "#version 300 es                                        \n"
        "uniform vec2 u_resolution;                             \n"
        "layout(location = 0) in vec2 a_position;               \n"
        "layout(location = 1) in vec2 a_texCoord;               \n"
        "layout(location = 2) in vec4 a_color;                  \n"
        "out vec2 v_texCoord;                                   \n"
        "out vec4 v_color;                                      \n"
        "void main()                                            \n"
        "{                                                      \n"
        "   vec2 pos = a_position;                              \n"
        "   vec2 clipSpace = ((pos / u_resolution) * 2.0) - 1.0;\n"
        "   gl_Position = vec4(clipSpace * vec2(1, -1), 0, 1);  \n"
        "   v_texCoord = a_texCoord;                            \n"
        "   v_color = a_color;                                  \n"
        "}";

    // Fragment shader. It discards any pixel whose alpha value is < 0.1.
    // See https://learnopengl.com/Advanced-OpenGL/Blending
    static const GLchar* fragmentShaderCode =
        "#version 300 es                                                \n"
        "precision mediump float;                                       \n"
        "in vec2 v_texCoord;                                            \n"
        "in vec4 v_color;                                               \n"
        "layout(location = 0) out vec4 outColor;                        \n"
        "uniform sampler2D s_texture;                                   \n"
        "void main()                                                    \n"
        "{                                                              \n"
        "  vec4 texColour = v_color * texture(s_texture, v_texCoord);   \n"
        "  if (texColour.a < 0.1)                                       \n"
        "    discard;                                                   \n"
        "  outColor = texColour;                                        \n"
        "}";

    // Create and compile the vertex shader.
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!glIsShader(vertexShader))
    {
        LOG("Failed to create vertex shader");
        return 0;
    }
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);
    {
        GLint params = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &params);
        if (params != GL_TRUE)
        {
            LOG("Failed to compile vertex shader");
            return 0;
        }
    }

    // Create and compile the fragment shader.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!glIsShader(fragmentShader))
    {
        LOG("Failed to create fragment shader");
        return 0;
    }
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);
    {
        GLint params = 0;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &params);
        if (params != GL_TRUE)
        {
            LOG("Failed to compile fragment shader");
            return 0;
        }
    }

    // Create the program, attach the shaders to it, and link it
    program = glCreateProgram();
    if (!glIsProgram(program))
    {
        LOG("Failed to create shader program");
        return program;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Delete the shaders as the program owns them now.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


void TearDownShaders()
{
    if (glIsProgram(program))
    {
        glDeleteProgram(program);
        program = 0;
    }
}


// --- Main program ----------------------------------------------------------------------------------------------------


// Window information.
const GLuint WIDTH = 512;
const GLuint HEIGHT = 512;
const char* const TITLE = "GLFW Skeleton App";

// Virtual screen information.
const GLsizei VIRTUAL_WIDTH = WIDTH / 2;
const GLsizei VIRTUAL_HEIGHT = HEIGHT / 2;


// Show / hide the Windows console.
void ToggleConsole()
{
    static bool isConsoleHidden = false;

    if (isConsoleHidden)
    {
        HWND activeWindow = GetActiveWindow();
        ShowWindow(GetConsoleWindow(), 1);
        SetFocus(activeWindow);
    }
    else
    {
        ShowWindow(GetConsoleWindow(), 0);
    }
    isConsoleHidden = !isConsoleHidden;
}


void GLAPIENTRY OnDebugMessage(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    LOG("GL callback: type = " << type << ", severity = " << severity << ", message = " << message);
}


// Called by GLFW whenever a key is pressed or released.
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        ToggleConsole();
    }
}


// Called by GLFW whenever a joystick / gamepad is connected or disconnected.
void OnJoystickEvent(int joystickId, int event)
{
    if (event == GLFW_CONNECTED)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - connected");
        }
    }
    else if (event == GLFW_DISCONNECTED)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - disconnected");
        }
    }
}


int main()
{
    // Initialize GLFW.
    glfwInit();

    // Set the options for GLFW.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow and make its context current.
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (window == nullptr)
    {
        LOG("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    // Load modern OpenGL mappings.
    if (!gladLoadGL())
    {
        LOG("Failed to initialize OpenGL context");
        return -1;
    }
    LOG("Using OpenGL " << GLVersion.major << "." << GLVersion.minor);

    // Enable OpenGL debugging. Requires OpenGL 4.3 or greater.
    if (glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OnDebugMessage, nullptr);
    }

    // Create a shader program.
    auto shaderProgram = InitializeShaders();
    LOG("Shader program " << shaderProgram);

    // Wire up some GLFW callbacks.
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetJoystickCallback(OnJoystickEvent);

    // Enumerate the gamepads.
    for (int joystickId = GLFW_JOYSTICK_1; joystickId <= GLFW_JOYSTICK_LAST; joystickId++)
    {
        if (glfwJoystickIsGamepad(joystickId))
        {
            LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - found");
        }
    }

    // Load a texture.
    EmTexture texture = emLoadTextureFromFile("../assets/sprite_tiles.png");

    EmBatch* batch = emInitializeBatch(shaderProgram);

    // Loop.
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        glfwPollEvents();

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the viewport position and size.
        glViewport(0, 0, WIDTH, HEIGHT);

        // Draw the batch.
        emBeginBatch(batch, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

        // Fill the screen with the texture that we loaded previously.
        EmQuad quad;
        EmRect2v srcRect;
        srcRect.position = { 0, 0 };
        srcRect.size = { (GLfloat) texture.w, (GLfloat) texture.h };
        EmVec2f dstSize = { VIRTUAL_WIDTH, VIRTUAL_HEIGHT };
        emMakeQuad(&texture, srcRect, dstSize, false, false, &quad);
        EmPosition position;
        position.position.x = VIRTUAL_WIDTH / 2;
        position.position.y = VIRTUAL_WIDTH / 2;
        position.centre.x = 0.0f;
        position.centre.y = 0.0f;
        position.rotation.cos = 1.0f;
        position.rotation.sin = 0.0f;
        position.scale.x = 1.0f;
        position.scale.y = 1.0f;
        emAddQuadToBatch(batch, &quad, &position);

        emEndBatch(batch);

        // Swap buffers.
        glfwSwapBuffers(window);
    }

    emTearDownBatch();
    TearDownShaders();

    // Terminate GLFW, clearing its resources.
    glfwTerminate();

    return 0;
}
