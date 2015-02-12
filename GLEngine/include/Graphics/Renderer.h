#pragma once

#include "Graphics/LightManager.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/Core/GLConstantBuffer.h"
#include "Graphics/GL/Core/GLShader.h"
#include "Graphics/GL/Core/GLTexture.h"
#include "Graphics/GL/Core/GLTextureBuffer.h"
#include "Graphics/GL/Core/GLUniform.h"
#include "Graphics/GL/Tech/ClusteredShading.h"

class Renderer
{
public:

	Renderer();
	~Renderer();

	void drawFrame(const PerspectiveCamera& camera);
	void resizeScreen(uint width, uint height);

private:
	/*
	GLTexture m_dfvTexture;
	GLMesh m_mesh;
	GLShader m_modelShader;

	LightManager m_lightManager;
	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;

	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;

	GLUniform<glm::mat3> m_normalMatrixUniform;
	GLUniform<glm::mat4> m_viewMatrixUniform;
	GLUniform<glm::mat4> m_mvpMatrixUniform;
	*/
};