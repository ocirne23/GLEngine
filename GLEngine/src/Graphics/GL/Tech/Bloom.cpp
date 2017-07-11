#include "Graphics/GL/Tech/Bloom.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const BLOOM_FRAG_SHADER_PATH = "Shaders/Bloom/bloom.frag";

END_UNNAMED_NAMESPACE()

void Bloom::initialize(uint a_xRes, uint a_yRes)
{
	m_bloomResultTexture.initialize(GLTexture::ESizedFormat::RGB8, a_xRes, a_yRes, GLConfig::getMultisampleType());
	m_bloomResultFBO.initialize(GLConfig::getMultisampleType());
	m_bloomResultFBO.addFramebufferTexture(m_bloomResultTexture);
	m_gaussianBlur.initialize(GaussianBlur::EBlurValueType::VEC3, a_xRes, a_yRes);

	reloadShader();

	m_initialized = true;
}

GLFramebuffer& Bloom::getBloomResultFBO(GLFramebuffer& a_sceneFBO)
{
	a_sceneFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Blur));

	m_bloomResultFBO.begin();
	QuadDrawer::drawQuad(m_bloomShader);
	m_bloomResultFBO.end();

	m_gaussianBlur.blurFBO(m_bloomResultFBO);

	return m_bloomResultFBO;
}

void Bloom::reloadShader()
{
	m_bloomShader.initialize(QUAD_VERT_SHADER_PATH, BLOOM_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_gaussianBlur.reloadShader();
}
