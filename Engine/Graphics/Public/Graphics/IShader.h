#pragma once

enum class EShaderStageType
{
	Vertex,
	Fragment,
	Geometry
};

class IShaderStage;

class IShader
{
public:

	virtual void initialize() = 0;
	virtual void addShaderStage(IShaderStage& stage, EShaderStageType type) = 0;

protected:

	friend class IContext;
	virtual ~IShader() {}
};