#pragma once

#include "Core.h"
#include "Graphics/LightManager.h"
#include "Graphics/Scene/Light.h"
#include "Graphics/Scene/Model.h"

#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/Core/GLShader.h"
#include "Graphics/GL/Core/GLTexture.h"
#include "Graphics/GL/Tech/ClusteredShading.h"

#include "Utils/MatForward.h"
#include "Utils/VecForward.h"

#include "rde/vector.h"

class PerspectiveCamera;

class GLScene
{
public:

	GLScene();
	~GLScene();
	GLScene(const GLScene& copy) = delete;

	void render(const PerspectiveCamera& camera, const rde::vector<Model>& models, const rde::vector<Light>& lights);

private:

	GLShader m_uberShader;
	GLTexture m_dfvTexture;
	ClusteredShading m_clusteredShading;
	LightManager m_lightManager;

	rde::vector<GLMesh> m_meshes;
};