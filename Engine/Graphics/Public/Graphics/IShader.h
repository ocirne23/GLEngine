#pragma once

#include "GraphicsAPI.h"
#include "Core/Pimpl.h"

class GLShader;
class IShaderStage;

class GRAPHICS_API IShader
{
public:

	IShader();
	explicit operator GLShader&() { return *m_impl; }

	void initialize();
	void addShaderStage(IShaderStage& stage);

private:

	Pimpl<GLShader> m_impl;
};