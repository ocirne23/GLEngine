#pragma once

#include "Graphics/IContext.h"

using ContextHandle = void*;

class GLContext : public IContext
{
public:

	virtual EContextType getType() const override { return EContextType::OPENGL; }

	virtual void setViewportSize(uint width, uint height) override;
	virtual void clearColor(const glm::vec4 & color, bool clearDepth) override;

	virtual owner<IBuffer*> createBuffer(const EBufferType& type) override;
	virtual void destroyBuffer(owner<IBuffer*> buffer) override;

	virtual owner<IFramebuffer*> createFramebuffer() override;
	virtual void destroyFramebuffer(owner<IFramebuffer*> framebuffer) override;

	virtual owner<IVertexAttributes*> createVertexAttributes() override;
	virtual void destroyVertexAttributes(owner<IVertexAttributes*> vertexAttributes) override;

	virtual owner<IShader*> createShader() override;
	virtual void destroyShader(owner<IShader*> shader) override;

private:

	friend class Window;
	GLContext(Window& window);
	~GLContext() override;
	GLContext(const GLContext&) = delete;

#ifdef DEBUG_BUILD
private:
	int md_numBuffersCreated = 0;
	int md_numFramebuffersCreated = 0;
	int md_numVertexAttributesCreated = 0;
	int md_numShadersCreated = 0;
#endif

private:
	ContextHandle m_context = nullptr;
};