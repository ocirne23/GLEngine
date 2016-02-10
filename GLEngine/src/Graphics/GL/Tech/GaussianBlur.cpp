#include "Graphics/GL/Tech/GaussianBlur.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const BLUR_FRAG_SHADER_PATH = "Shaders/Blur/gaussianblur.frag";

END_UNNAMED_NAMESPACE()

void GaussianBlur::initialize(EBlurValueType a_type, uint a_xRes, uint a_yRes)
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
	m_blurShader.initialize(QUAD_VERT_SHADER_PATH, BLUR_FRAG_SHADER_PATH, &blurDefines);
	m_blurXResultFBO.initialize();
	m_blurXResultFBO.addFramebufferTexture(format, GLFramebuffer::EAttachment::COLOR0, a_xRes, a_yRes);

	m_pixelXOffset = 1.0f / float(a_xRes);
	m_pixelYOffset = 1.0f / float(a_yRes);

	m_initialized = true;
}

void GaussianBlur::blurFBO(GLFramebuffer& a_blurredFBO)
{
	assert(m_initialized);

	a_blurredFBO.bindTexture(0, GLConfig::BLUR_TEXTURE_BINDING_POINT);

	m_blurShader.begin();

	m_blurShader.setUniform2f("u_pixelOffset", glm::vec2(m_pixelXOffset, 0.0f));
	m_blurXResultFBO.begin();
	QuadDrawer::drawQuad(m_blurShader);
	m_blurXResultFBO.end();

	m_blurShader.setUniform2f("u_pixelOffset", glm::vec2(0.0f, m_pixelYOffset));
	m_blurXResultFBO.bindTexture(0, GLConfig::BLUR_TEXTURE_BINDING_POINT);
	a_blurredFBO.begin();
	QuadDrawer::drawQuad(m_blurShader);
	a_blurredFBO.end();

	m_blurShader.end();
}

