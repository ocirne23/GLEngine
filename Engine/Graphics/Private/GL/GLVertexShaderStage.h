#pragma once

#include "Graphics/IVertexShaderStage.h"

class GLVertexShaderStage : public IVertexShaderStage
{
public:



private:

	friend class GLContext;
	GLVertexShaderStage() {}
	~GLVertexShaderStage() override {}
	GLVertexShaderStage(const GLVertexShaderStage&) = delete;
};