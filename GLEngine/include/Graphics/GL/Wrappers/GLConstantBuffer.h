#pragma once

#include "Core.h"

#include "Graphics/GL/GLTypes.h"

class GLShader;

class GLConstantBuffer
{
public:
    friend class GLStateBuffer;

    GLConstantBuffer() {}
    ~GLConstantBuffer();
    GLConstantBuffer(const GLConstantBuffer& copy) = delete;

    void initialize(const GLShader& shader, GLuint bindingPoint, const char* blockName, GLenum drawUsage);
    void upload(uint numBytes, const void* data);
    void bind();

private:

    bool m_initialized = false;
    const GLShader* m_shader = NULL;
    GLenum m_drawUsage = 0;
    GLuint m_ubo = 0;
    GLuint m_bindingPoint = 0;
    GLuint m_uboIndex = 0;
};