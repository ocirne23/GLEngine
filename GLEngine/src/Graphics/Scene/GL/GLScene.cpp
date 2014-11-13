#include "Graphics\Scene\GL\GLScene.h"

GLScene::GLScene()
{

}

GLScene::~GLScene()
{

}

void GLScene::render(const PerspectiveCamera& camera, const rde::vector<Model>& models)
{
	m_dfvTexture.bind(GLAppVars::TextureUnits_DFV_TEXTURE);
	m_uberShader.begin();
	{
		m_lightManager.update(camera);
		m_clusteredShading.update(camera, m_lightManager.getViewspaceLightPositionRangeListBegin(), m_lightManager.getNumLights());

		m_uberShader.setUniform3f("u_eyePos", glm::vec3(camera.m_viewMatrix * glm::vec4(camera.m_position, 1.0f)));
		m_uberShader.setUniformMatrix4f("u_mv", camera.m_viewMatrix);
		m_uberShader.setUniformMatrix4f("u_mvp", camera.m_combinedMatrix);
		m_uberShader.setUniformMatrix3f("u_normalMat", glm::mat3(glm::inverse(glm::transpose(camera.m_viewMatrix))));

		for (const Model& model : models)
		{
			m_uberShader.setUniformMatrix4f("u_transform", model.transform);
			model.mesh->render();
		}
	}
	m_uberShader.end();
}