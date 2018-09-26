#pragma once

#include "Graphics/IShader.h"

class GLShader : public IShader
{
public:

	virtual void initialize();
	virtual void addShaderStage();

private:

	friend class GLContext;
	GLShader() {}
	~GLShader() override {}
	GLShader(const GLShader&) = delete;
};