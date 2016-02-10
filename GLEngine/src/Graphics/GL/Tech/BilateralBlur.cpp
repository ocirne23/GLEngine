#include "Graphics/GL/Tech/BilateralBlur.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const BLURX_FRAG_SHADER_PATH = "Shaders/Blur/bilateralblurx.frag";
const char* const BLURY_FRAG_SHADER_PATH = "Shaders/Blur/bilateralblury.frag";

END_UNNAMED_NAMESPACE()

void BilateralBlur::initialize(EBlurValueType a_type, uint a_xRes, uint a_yRes)
{
	GLFramebuffer::ESizedFormat format = GLFramebuffer::ESizedFormat::RGB8;
	eastl::vector<eastl::string> blurDefines = GLConfig::getGlobalShaderDefines();
	switch (a_type)
	{
	case EBlurValueType::FLOAT:
	{
		blurDefines.push_back(eastl::string("VALTYPE float"));
		blurDefines.push_back(eastl::string("VALACCESSOR r"));
		format = GLFramebuffer::ESizedFormat::R8;
		break;
	}
	case EBlurValueType::VEC2:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec2"));
		blurDefines.push_back(eastl::string("VALACCESSOR rg"));
		format = GLFramebuffer::ESizedFormat::RG8;
		break;
	}
	case EBlurValueType::VEC3:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec3"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgb"));
		format = GLFramebuffer::ESizedFormat::RGB8;
		break;
	}
	case EBlurValueType::VEC4:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec4"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgba"));
		format = GLFramebuffer::ESizedFormat::RGBA8;
		break;
	}
	}
	m_blurXShader.initialize(QUAD_VERT_SHADER_PATH, BLURX_FRAG_SHADER_PATH, &blurDefines);
	m_blurYShader.initialize(QUAD_VERT_SHADER_PATH, BLURY_FRAG_SHADER_PATH, &blurDefines);
	m_blurXResultFBO.initialize();
	m_blurXResultFBO.addFramebufferTexture(format, GLFramebuffer::EAttachment::COLOR0, a_xRes, a_yRes);

	m_initialized = true;
}

void BilateralBlur::blurFBO(GLFramebuffer& a_blurredFBO, GLFramebuffer& a_depthFBO)
{
	assert(m_initialized);

	a_depthFBO.bindDepthTexture(GLConfig::DEPTH_TEXTURE_BINDING_POINT);
	a_blurredFBO.bindTexture(0, GLConfig::BLUR_TEXTURE_BINDING_POINT);

	m_blurXResultFBO.begin();
	QuadDrawer::drawQuad(m_blurXShader);
	m_blurXResultFBO.end();

	m_blurXResultFBO.bindTexture(0, GLConfig::BLUR_TEXTURE_BINDING_POINT);
	a_blurredFBO.begin();
	QuadDrawer::drawQuad(m_blurYShader);
	a_blurredFBO.end();
}

