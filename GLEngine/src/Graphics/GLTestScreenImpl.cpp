#include "Graphics/GLTestScreenImpl.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"
#include "Input/Input.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

const glm::vec4 CLEAR_COLOR(1.0f, 0.0f, 0.0f, 1.0f);

GLShader s_shader;
GLVertexBuffer s_vertexBuffer;
GLStateBuffer s_stateBuffer;
GLTexture s_texture;
GLTextureArray s_textureArray;

END_UNNAMED_NAMESPACE()


GLTestScreenImpl::GLTestScreenImpl()
{
	float quad[] =
	{ // Position				Texcoords
		-0.9f,	0.9f, 0.0f,		0.0f, 0.0f,
		-0.9f, -0.9f, 0.0f,		1.0f, 0.0f,
		 0.9f,	0.9f, 0.0f,		0.0f, 1.0f,
		 0.9f, -0.9f, 0.0f,		1.0f, 1.0f
	};

	VertexAttribute attributes[] = 
	{
		{ 0, "POSITION", VertexAttribute::EFormat::FLOAT, 3 },
		{ 1, "TEXCOORDS", VertexAttribute::EFormat::FLOAT, 2 }
	};

	s_texture.initialize("Models/palace/palace.obj-atlas-0.da");
	//s_texture.initialize("UI/button.9.png.da");
	s_texture.bind(0);

	//rde::vector<rde::string> images;
	//images.push_back(rde::string("Models/palace/palace.obj-atlas-0.da"));
	//s_textureArray.initialize(images);
	//s_textureArray.bind(1);

	s_stateBuffer.initialize();
	s_stateBuffer.begin();

	s_shader.initialize("Shaders/quad.vert", "Shaders/quad.frag");
	s_shader.begin();
	s_shader.setUniform1i("u_tex", 0);
	s_shader.setUniform1i("u_textureArray", 1);
	s_shader.end();

	s_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	s_vertexBuffer.setVertexAttributes(ARRAY_SIZE(attributes), attributes);
	s_vertexBuffer.upload(sizeof(quad), quad);

	s_stateBuffer.end();
}

GLTestScreenImpl::~GLTestScreenImpl()
{

}

void GLTestScreenImpl::render(float a_deltaSec)
{
	GLEngine::graphics->clear(CLEAR_COLOR);

	s_shader.begin();
	s_stateBuffer.begin();
	s_texture.bind(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	s_stateBuffer.end();
	s_shader.end();

	GLEngine::graphics->swap();

	if (GLEngine::input->isKeyPressed(EKey::ESCAPE))
		GLEngine::shutdown();
}