#pragma once

#pragma once 

#include "Core.h"
#include "Graphics/Scene/Light.h"
#include "Graphics/Scene/Model.h"

#include "Graphics\GL\GLLightManager.h"
#include "Graphics\GL\GLMesh.h""
#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\Tech\ClusteredShading.h"

#include "Utils\MatForward.h"
#include "Utils\VecForward.h"

#include "rde\vector.h"

class PerspectiveCamera;

class GLScene
{
public:

	GLScene();
	~GLScene();
	GLScene(const GLScene& copy) = delete;

	void render(const PerspectiveCamera& camera, const rde::vector<Model>& models);

private:

	GLShader m_uberShader;
	GLLightManager m_lightManager;
	ClusteredShading m_clusteredShading;

	rde::vector<GLMesh> m_meshes;
};