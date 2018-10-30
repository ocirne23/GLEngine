#pragma once

#include "GraphicsAPI.h"

class GLShader;

class GRAPHICS_API IShader
{
public:

	IShader();
	IShader(const IShader&) = delete;
	~IShader();
	explicit operator GLShader&() { return *m_impl; }

	void initialize();
	void addShaderStage(IShaderStage& stage);

private:

	owner<GLShader*> m_impl = nullptr;
};