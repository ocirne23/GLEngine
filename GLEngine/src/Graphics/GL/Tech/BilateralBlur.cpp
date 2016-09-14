#include "Graphics/GL/Tech/BilateralBlur.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "Utils/StringUtils.h"

#include <glm/glm.hpp>
#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const BLURX_FRAG_SHADER_PATH = "Shaders/Blur/bilateralblurx.frag";
const char* const BLURY_FRAG_SHADER_PATH = "Shaders/Blur/bilateralblury.frag";

END_UNNAMED_NAMESPACE()

void BilateralBlur::initialize(EBlurValueType a_type, uint a_xRes, uint a_yRes, uint a_blurRadius, uint a_blurTexWidth, uint a_blurTexHeight)
{
	m_type = a_type;
	m_blurRadius = float(a_blurRadius);
	m_xRes = a_xRes;
	m_yRes = a_yRes;
	m_blurTexWidth = a_blurTexWidth;
	m_blurTexHeight = a_blurTexHeight;

	reloadShader();

	m_blurXResultFBO.initialize(GLConfig::getMultisampleType());
	m_blurXResultFBO.addFramebufferTexture(m_format, GLFramebuffer::EAttachment::COLOR0, a_xRes, a_yRes);

	m_initialized = true;
}

void BilateralBlur::reloadShader()
{
	eastl::vector<eastl::string> blurDefines = GLConfig::getGlobalShaderDefines();
	switch (m_type)
	{
	case EBlurValueType::FLOAT:
	{
		blurDefines.push_back(eastl::string("VALTYPE float"));
		blurDefines.push_back(eastl::string("VALACCESSOR r"));
		m_format = GLFramebuffer::ESizedFormat::R8;
		break;
	}
	case EBlurValueType::VEC2:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec2"));
		blurDefines.push_back(eastl::string("VALACCESSOR rg"));
		m_format = GLFramebuffer::ESizedFormat::RG8;
		break;
	}
	case EBlurValueType::VEC3:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec3"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgb"));
		m_format = GLFramebuffer::ESizedFormat::RGB8;
		break;
	}
	case EBlurValueType::VEC4:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec4"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgba"));
		m_format = GLFramebuffer::ESizedFormat::RGBA8;
		break;
	}
	}
	blurDefines.push_back("BLUR_KERNEL_RADIUS " + StringUtils::to_string(m_blurRadius));
	blurDefines.push_back("SHARPNESS " + StringUtils::to_string(m_sharpness));

	m_blurXShader.initialize(QUAD_VERT_SHADER_PATH, BLURX_FRAG_SHADER_PATH, &blurDefines);
	m_blurXShader.begin();
	glm::vec2 invScreenSize = glm::vec2(1.0f / m_xRes, 1.0f / m_yRes);
	glm::vec2 invBlurTexSize = glm::vec2(1.0f / m_blurTexWidth, 1.0f / m_blurTexHeight);
	m_blurXShader.setUniform2f("u_invScreenSize", invScreenSize);
	m_blurXShader.setUniform2f("u_blurTexScale", invScreenSize / invBlurTexSize);
	m_blurXShader.end();

	m_blurYShader.initialize(QUAD_VERT_SHADER_PATH, BLURY_FRAG_SHADER_PATH, &blurDefines);
	m_blurYShader.begin();
	m_blurYShader.setUniform2f("u_invScreenSize", glm::vec2(1.0f / m_xRes, 1.0f / m_yRes));
	m_blurYShader.end();
}

void BilateralBlur::blurFBO(GLFramebuffer& a_blurredFBO, GLFramebuffer& a_depthFBO)
{
	assert(m_initialized);

	a_depthFBO.bindDepthTexture(GLConfig::getTextureBindingPoint(GLConfig::ETextures::Depth));
	a_blurredFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Blur));

	m_blurXResultFBO.begin();
	QuadDrawer::drawQuad(m_blurXShader);
	m_blurXResultFBO.end();
	m_blurXResultFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Blur));
	a_blurredFBO.begin();
	QuadDrawer::drawQuad(m_blurYShader);
	a_blurredFBO.end();
}

