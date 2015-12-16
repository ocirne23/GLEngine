#include "Graphics/GL/Tech/Bloom.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const BLOOM_FRAG_SHADER_PATH = "Shaders/Bloom/bloom.frag";

END_UNNAMED_NAMESPACE()

void Bloom::initialize(uint a_xRes, uint a_yRes)
{
	reloadShader();
	m_bloomResultFBO.initialize();
	m_bloomResultFBO.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, a_xRes, a_yRes);
	m_gaussianBlur.initialize(GaussianBlur::EBlurValueType::VEC3, a_xRes, a_yRes);
	m_initialized = true;
}

GLFramebuffer& Bloom::getBloomResultFBO(GLFramebuffer& a_sceneFBO)
{
	a_sceneFBO.bindTexture(0, GLConfig::BLUR_TEXTURE_BINDING_POINT);

	m_bloomResultFBO.begin();
	QuadDrawer::drawQuad(m_bloomShader);
	m_bloomResultFBO.end();

	m_gaussianBlur.blurFBO(m_bloomResultFBO);

	return m_bloomResultFBO;
}

void Bloom::reloadShader()
{
	m_bloomShader.initialize(QUAD_VERT_SHADER_PATH, BLOOM_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
}
