#pragma once

#include "Graphics/GL/Wrappers/GLCubeMap.h"
#include "Graphics/Utils/PerspectiveCamera.h"

#include <glm/glm.hpp>

class CubeMapGen
{
public:

	enum class ECubeMapFace { RIGHT = 0, LEFT, TOP, BOTTOM, BACK, FRONT };

	CubeMapGen() {}
	CubeMapGen(const CubeMapGen& copy) = delete;
	~CubeMapGen();

	void initialize(uint width, uint height);
	PerspectiveCamera& beginRenderCubeMapFace(const glm::vec3& position, ECubeMapFace face);
	void endRenderCubeMapFace();
	GLCubeMap& getCubeMap() { return m_cubeMap; }

private:

	bool m_initialized = false;
	bool m_begun       = false;
	uint m_fboID       = 0;
	PerspectiveCamera m_camera;
	GLCubeMap m_cubeMap;
};