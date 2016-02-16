#include "Graphics/GL/Tech/CubeMapGen.h"

#include "Graphics/GL/GL.h"

BEGIN_UNNAMED_NAMESPACE()

const float CAMERA_ANGLE = 90.0f;
const float CAMERA_NEAR = 0.1f;
const float CAMERA_FAR = 1000.0f;

const glm::vec3 DIRECTIONS[] = {
	glm::vec3(1, 0, 0),
	glm::vec3(-1, 0, 0),
	glm::vec3(0.0000001, 1, 0),
	glm::vec3(0.0000001, -1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1)
};

END_UNNAMED_NAMESPACE()

void CubeMapGen::initialize(uint a_width, uint a_height)
{
	if (m_initialized)
		glDeleteFramebuffers(1, &m_fboID);
	glGenFramebuffers(1, &m_fboID);
	m_cubeMap.initialize(a_width, a_height);
	m_initialized = true;
}

CubeMapGen::~CubeMapGen()
{
	if (m_initialized)
		glDeleteFramebuffers(1, &m_fboID);
}

PerspectiveCamera& CubeMapGen::beginRenderCubeMapFace(const glm::vec3& a_position, ECubeMapFace a_face)
{
	assert(!m_begun);
	m_camera.initialize(float(m_cubeMap.getWidth()), float(m_cubeMap.getHeight()), CAMERA_ANGLE, CAMERA_NEAR, CAMERA_FAR);
	m_camera.setPosition(a_position);
	m_camera.lookAtDir(DIRECTIONS[uint(a_face)]);

	glViewport(0, 0, m_cubeMap.getWidth(), m_cubeMap.getHeight());

	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + uint(a_face), m_cubeMap.getTextureID(), 0);
	m_begun = true;

	return m_camera;
}

void CubeMapGen::endRenderCubeMapFace()
{
	assert(m_begun);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_begun = false;
}
