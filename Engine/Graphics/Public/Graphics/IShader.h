#pragma once

class IVertexShaderStage;
class IFragmentShaderStage;

class IShader
{
public:

	virtual void initialize() = 0;
	virtual void setShaderStage(IVertexShaderStage& stage) = 0;
	virtual void setShaderStage(IFragmentShaderStage& stage) = 0;

protected:

	friend class IContext;
	virtual ~IShader() {}
};