#include "Utils/GLTestScreenImpl.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"
#include "Graphics/GL/Utils/QuadDrawUtils.h"
#include "Graphics/GL/Utils/CheckGLError.h"
#include "Input/Input.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

const glm::vec4 CLEAR_COLOR(1.0f, 0.0f, 0.0f, 1.0f);

GLTexture s_texture;
GLTextureArray s_textureArray;

END_UNNAMED_NAMESPACE()

GLTestScreenImpl::GLTestScreenImpl()
{
	s_texture.initialize("Utils/test.png.da", GLTexture::ETextureMinFilter::NEAREST, GLTexture::ETextureMagFilter::NEAREST);
	CHECK_GL_ERROR();
}

GLTestScreenImpl::~GLTestScreenImpl()
{

}

void GLTestScreenImpl::render(float a_deltaSec)
{
	s_texture.bind(0);

	GLEngine::graphics->clear(CLEAR_COLOR);
	QuadDrawUtils::drawQuad();
	GLEngine::graphics->swap();

	if (GLEngine::input->isKeyPressed(EKey::ESCAPE))
		GLEngine::shutdown();
}