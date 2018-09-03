#pragma once

#include "Frustum.h"

#include <glm/gtx/fast_square_root.hpp>

BEGIN_UNNAMED_NAMESPACE()

enum FrustumPlane
{
	FRUSTUM_PLANE_NEAR = 0,
	FRUSTUM_PLANE_FAR = 1,
	FRUSTUM_PLANE_LEFT = 2,
	FRUSTUM_PLANE_RIGHT = 3,
	FRUSTUM_PLANE_TOP = 4,
	FRUSTUM_PLANE_BOTTOM = 5
};

END_UNNAMED_NAMESPACE();

void Frustum::calculateFrustum(const glm::mat4& vpMatrix)
{
	glm::mat4 mat = glm::transpose(vpMatrix);
	m_planes[FRUSTUM_PLANE_LEFT].normal.x = mat[3][0] + mat[0][0];
	m_planes[FRUSTUM_PLANE_LEFT].normal.y = mat[3][1] + mat[0][1];
	m_planes[FRUSTUM_PLANE_LEFT].normal.z = mat[3][2] + mat[0][2];
	m_planes[FRUSTUM_PLANE_LEFT].d = mat[3][3] + mat[0][3];

	m_planes[FRUSTUM_PLANE_RIGHT].normal.x = mat[3][0] - mat[0][0];
	m_planes[FRUSTUM_PLANE_RIGHT].normal.y = mat[3][1] - mat[0][1];
	m_planes[FRUSTUM_PLANE_RIGHT].normal.z = mat[3][2] - mat[0][2];
	m_planes[FRUSTUM_PLANE_RIGHT].d = mat[3][3] - mat[0][3];

	m_planes[FRUSTUM_PLANE_TOP].normal.x = mat[3][0] - mat[1][0];
	m_planes[FRUSTUM_PLANE_TOP].normal.y = mat[3][1] - mat[1][1];
	m_planes[FRUSTUM_PLANE_TOP].normal.z = mat[3][2] - mat[1][2];
	m_planes[FRUSTUM_PLANE_TOP].d = mat[3][3] - mat[1][3];

	m_planes[FRUSTUM_PLANE_BOTTOM].normal.x = mat[3][0] + mat[1][0];
	m_planes[FRUSTUM_PLANE_BOTTOM].normal.y = mat[3][1] + mat[1][1];
	m_planes[FRUSTUM_PLANE_BOTTOM].normal.z = mat[3][2] + mat[1][2];
	m_planes[FRUSTUM_PLANE_BOTTOM].d = mat[3][3] + mat[1][3];

	m_planes[FRUSTUM_PLANE_NEAR].normal.x = mat[3][0] + mat[2][0];
	m_planes[FRUSTUM_PLANE_NEAR].normal.y = mat[3][1] + mat[2][1];
	m_planes[FRUSTUM_PLANE_NEAR].normal.z = mat[3][2] + mat[2][2];
	m_planes[FRUSTUM_PLANE_NEAR].d = mat[3][3] + mat[2][3];

	m_planes[FRUSTUM_PLANE_FAR].normal.x = mat[3][0] - mat[2][0];
	m_planes[FRUSTUM_PLANE_FAR].normal.y = mat[3][1] - mat[2][1];
	m_planes[FRUSTUM_PLANE_FAR].normal.z = mat[3][2] - mat[2][2];
	m_planes[FRUSTUM_PLANE_FAR].d = mat[3][3] - mat[2][3];

	// Renormalise any normals which were not unit length
	for (int i = 0; i < 6; i++)
	{
		float length = glm::length(m_planes[i].normal);
		m_planes[i].normal /= length;
		m_planes[i].d /= length;
	}
}

bool Frustum::pointInFrustum(const glm::vec3& point) const
{
	for (int p = 0; p < 6; p++)
		if (m_planes[p].getSide(point) == Plane::Side::NEGATIVE)
			return false;
	return true;
}

bool Frustum::sphereInFrustum(const glm::vec3& point, float radius) const
{
	for (int p = 0; p < 6; p++)
		if (m_planes[p].getDistance(point) < -radius)
			return false;
	return true;
}

bool Frustum::aabbInFrustum(const glm::vec3& center, const glm::vec3& halfSize) const
{
	for (uint plane = 0; plane < 6; ++plane)
		if (m_planes[plane].getSide(center, halfSize) == Plane::Side::NEGATIVE)
			return false;
	return true;
}