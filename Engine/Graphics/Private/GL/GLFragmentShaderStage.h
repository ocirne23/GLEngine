#pragma once

#include "Graphics/IFragmentShaderStage.h"

class GLFragmentShaderStage : public IFragmentShaderStage
{
public:



private:

	friend class GLContext;
	GLFragmentShaderStage() {}
	~GLFragmentShaderStage() override {}
	GLFragmentShaderStage(const GLFragmentShaderStage&) = delete;
};