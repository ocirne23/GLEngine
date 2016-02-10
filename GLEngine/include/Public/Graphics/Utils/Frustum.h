#pragma once

#include <glm/glm.hpp>

class PerspectiveCamera;

class Frustum
{
public:

	Frustum() {}
	~Frustum() {}
	Frustum(const Frustum& copy) = delete;

	void calculateFrustum(const glm::mat4& mvp);
	bool pointInFrustum(const glm::vec3& point) const;
	bool sphereInFrustum(const glm::vec3& point, float radius) const;

public:

	static bool aabbInFrustum(const glm::vec3& point, const glm::vec3& extents, const glm::mat4& frustumMatrix);
	
	struct Corners
	{
		enum { NTL = 0, NTR, NBL, NBR, FTL, FTR, FBL, FBR, NUM_CORNERS };
		glm::vec3 corners[NUM_CORNERS];
	};
	static Corners calculateCorners(const PerspectiveCamera& camera);

public:

	glm::vec4 m_planes[6];
};