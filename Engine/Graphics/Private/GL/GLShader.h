#pragma once

#include "Graphics/IShader.h"

class GLShader : public IShader
{
public:

	virtual void initialize() override;
	virtual void setShaderStage(IVertexShaderStage& stage) override;
	virtual void setShaderStage(IFragmentShaderStage& stage) override;

private:


private:

	friend class GLContext;
	GLShader() {}
	~GLShader() override {}
	GLShader(const GLShader&) = delete;
};