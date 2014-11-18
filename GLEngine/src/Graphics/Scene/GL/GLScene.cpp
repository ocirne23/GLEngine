#include "Graphics/Scene/GL/GLScene.h"

#include "Graphics/PerspectiveCamera.h"

GLScene::GLScene() : m_lightManager(50)
{
	m_dfvTexture.initialize("Utils/ggx-helper-dfv.da", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

GLScene::~GLScene()
{

}

void GLScene::begin(const PerspectiveCamera& a_camera)
{
	m_dfvTexture.bind(GLAppVars::TextureUnits_DFV_TEXTURE);
	m_uberShader.begin();

	m_clusteredShading.update(a_camera, m_lightManager.getLightPositionRanges(), m_lightManager.getNumLights());

	m_uberShader.setUniform3f("u_eyePos", glm::vec3(a_camera.m_viewMatrix * glm::vec4(a_camera.m_position, 1.0f)));
	m_uberShader.setUniformMatrix4f("u_mv", a_camera.m_viewMatrix);
	m_uberShader.setUniformMatrix4f("u_mvp", a_camera.m_combinedMatrix);
	m_uberShader.setUniformMatrix3f("u_normalMat", glm::mat3(glm::inverse(glm::transpose(a_camera.m_viewMatrix))));

}

void GLScene::end()
{
	m_uberShader.end();
}

void GLScene::render(const PerspectiveCamera& a_camera, const rde::vector<Model>& a_models, const rde::vector<Light>& a_lights)
{

		for (const Model& model : a_models)
		{
			m_uberShader.setUniformMatrix4f("u_transform", model.transform);
			model.mesh->render();
		}
	}
}