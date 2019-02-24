#pragma once

#include "GraphicsAPI.h"
#include "Core/Pimpl.h"
#include <EASTL/string.h>
#include <EASTL/vector.h>

enum class EShaderStageType
{
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

class GLShaderStage;

class GRAPHICS_API IShaderStage
{
public:

	IShaderStage();
	explicit operator GLShaderStage&() { return *m_impl; }

	struct RequiredDefine
	{
		eastl::string name;
		eastl::string defaultValue;
	};
	
	/*
	//virtual void initialize(EShaderStageType);
	void setRequiredDefines(const eastl::vector<RequiredDefine>& requiredDefines);
	const eastl::vector<RequiredDefine>& getRequiredDefines() const;
	EShaderStageType getType() const;
	*/
private:

	Pimpl<GLShaderStage> m_impl;
};