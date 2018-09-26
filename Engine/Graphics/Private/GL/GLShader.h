#pragma once

#include "Graphics/IShader.h"

class GLShader : public IShader
{
public:

	virtual void initialize() = 0;
	virtual void addShaderStage() = 0;

private:

	friend class GLContext;
	GLShader() {}
	~GLShader() override {}
	GLShader(const GLShader&) = delete;
};