#pragma once

#include <glm/glm.hpp>

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

public:

	glm::vec4 m_planes[6];
};