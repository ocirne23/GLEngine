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
	m_type = a_type;
	reloadShader();

	m_blurResultTexture.initialize(m_format, a_xRes, a_yRes, GLConfig::getMultisampleType());
	m_blurXResultFBO.initialize(GLConfig::getMultisampleType());
	m_blurXResultFBO.addFramebufferTexture(m_blurResultTexture);

	m_pixelXOffset = 1.0f / float(a_xRes);
	m_pixelYOffset = 1.0f / float(a_yRes);

	m_initialized = true;
}

void GaussianBlur::reloadShader()
{
	eastl::vector<eastl::string> blurDefines = GLConfig::getGlobalShaderDefines();
	switch (m_type)
	{
	case EBlurValueType::FLOAT:
	{
		blurDefines.push_back(eastl::string("VALTYPE float"));
		blurDefines.push_back(eastl::string("VALACCESSOR r"));
		m_format = GLTexture::ESizedFormat::R8;
		break;
	}
	case EBlurValueType::VEC2:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec2"));
		blurDefines.push_back(eastl::string("VALACCESSOR rg"));
		m_format = GLTexture::ESizedFormat::RG8;
		break;
	}
	case EBlurValueType::VEC3:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec3"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgb"));
		m_format = GLTexture::ESizedFormat::RGB8;
		break;
	}
	case EBlurValueType::VEC4:
	{
		blurDefines.push_back(eastl::string("VALTYPE vec4"));
		blurDefines.push_back(eastl::string("VALACCESSOR rgba"));
		m_format = GLTexture::ESizedFormat::RGBA8;
		break;
	}
	}

	m_blurShader.initialize(QUAD_VERT_SHADER_PATH, BLUR_FRAG_SHADER_PATH, &blurDefines);
}

void GaussianBlur::blurFBO(GLFramebuffer& a_blurredFBO)
{
	assert(m_initialized);

	a_blurredFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Blur));
	
	m_blurShader.begin();
	m_blurShader.setUniform2f("u_pixelOffset", glm::vec2(m_pixelXOffset, 0.0f));
	m_blurXResultFBO.begin();
	QuadDrawer::drawQuad(m_blurShader);
	m_blurXResultFBO.end();

	m_blurShader.setUniform2f("u_pixelOffset", glm::vec2(0.0f, m_pixelYOffset));
	m_blurXResultFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Blur));
	a_blurredFBO.begin();
	QuadDrawer::drawQuad(m_blurShader);
	a_blurredFBO.end();

	m_blurShader.end();
}

