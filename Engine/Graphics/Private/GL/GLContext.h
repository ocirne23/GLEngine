#pragma once

#include "Graphics/IContext.h"

using ContextHandle = void*;

class GLContext
{
public:

	GLContext();
	~GLContext();
	GLContext(const GLContext&) = delete;

	/*
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

	virtual owner<IVertexShaderStage*> createVertexShaderStage() override;
	virtual void destroyVertexShaderStage(owner<IVertexShaderStage*> shaderStage) override;

	virtual owner<IFragmentShaderStage*> createFragmentShaderStage() override;
	virtual void destroyFragmentShaderStageShader(owner<IFragmentShaderStage*> shaderStage) override;
	*/

private:

#ifdef DEBUG_BUILD
private:
	int md_numBuffersCreated = 0;
	int md_numFramebuffersCreated = 0;
	int md_numVertexAttributesCreated = 0;
	int md_numShadersCreated = 0;
	int md_numVertexShaderStagesCreated = 0;
	int md_numFragmentShaderStagesCreated = 0;
#endif

private:
	ContextHandle m_context = nullptr;
};