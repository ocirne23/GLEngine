#pragma once

#include "Graphics/GL/GLTypes.h"
#include "rde/vector.h"

class GLConstantBuffer;
class GLTextureBuffer;
class GLVertexBuffer;

/** VAO wrapper */
class GLStateBuffer
{
public:
    GLStateBuffer() {}
    ~GLStateBuffer();
    GLStateBuffer(const GLStateBuffer& copy) = delete;

    void initialize();
    void begin();
    void end();

    static bool isBegun() { return s_isBegun; }

private:

    bool m_initialized = false;
    bool m_isBegun = false;
    GLuint m_vao = 0;

private:

    static bool s_isBegun;
};