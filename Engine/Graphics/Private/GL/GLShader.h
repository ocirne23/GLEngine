#pragma once

#include "Core.h"
#include "Graphics/IShader.h"

class GLShader : public IShader
{
public:

	virtual void initialize() override;
	virtual void addShaderStage(IShaderStage& stage) override;

private:

	uint m_id = 0;

private:

	friend class GLContext;
	GLShader() {}
	~GLShader() override {}
	GLShader(const GLShader&) = delete;
};