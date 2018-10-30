#include "GLShaderStage.h"

void GLShaderStage::setRequiredDefines(const eastl::vector<RequiredDefine>& requiredDefines)
{
	m_requiredDefines.insert(m_requiredDefines.end(), requiredDefines.begin(), requiredDefines.end());
}

const eastl::vector<IShaderStage::RequiredDefine>& GLShaderStage::getRequiredDefines() const
{
	return m_requiredDefines;
}

EShaderStageType GLShaderStage::getType() const
{
	return EShaderStageType();
}

uint GLShaderStage::getID()
{
	return m_id;
}
