#pragma once

#include "Graphics/Utils/Plane.h"

#include <glm/glm.hpp>

class PerspectiveCamera;

class Frustum
{
public:

	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR = 1,
		FRUSTUM_PLANE_LEFT = 2,
		FRUSTUM_PLANE_RIGHT = 3,
		FRUSTUM_PLANE_TOP = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};

public:

	Frustum() {}
	~Frustum() {}
	Frustum(const Frustum& copy) = delete;

	void calculateFrustum(const glm::mat4& vpMatrix);
	bool pointInFrustum(const glm::vec3& point) const;
	bool sphereInFrustum(const glm::vec3& point, float radius) const;
	bool aabbInFrustum(const glm::vec3& min, const glm::vec3& max) const;

public:

	Plane m_planes[6];
};