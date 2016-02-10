#include "Graphics/Utils/Frustum.h"

#include "Graphics/Utils/PerspectiveCamera.h"

#include <glm/gtx/fast_square_root.hpp>

BEGIN_UNNAMED_NAMESPACE()

bool extentSignedTest(const glm::vec4& a_p, const glm::vec3& a_center, const glm::vec3& a_extent)
{
	const float pDoTCenter = glm::dot(glm::vec3(a_p), a_center);
	const float absPdotExtent = glm::dot(glm::abs(glm::vec3(a_p)), a_extent);
	return (pDoTCenter + absPdotExtent) < -a_p.z;
}

END_UNNAMED_NAMESPACE()

void Frustum::calculateFrustum(const glm::mat4& a_mvp)
{
	float t;
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

	const glm::mat4 trans = glm::transpose(a_mvp);

	m00 = trans[0][0];
	m01 = trans[0][1];
	m02 = trans[0][2];
	m03 = trans[0][3];

	m10 = trans[1][0];
	m11 = trans[1][1];
	m12 = trans[1][2];
	m13 = trans[1][3];

	m20 = trans[2][0];
	m21 = trans[2][1];
	m22 = trans[2][2];
	m23 = trans[2][3];

	m30 = trans[3][0];
	m31 = trans[3][1];
	m32 = trans[3][2];
	m33 = trans[3][3];

	m_planes[0].x = m30 - m00;
	m_planes[0].y = m31 - m01;
	m_planes[0].z = m32 - m02;
	m_planes[0].w = m33 - m03;

	t = glm::fastInverseSqrt(m_planes[0].x * m_planes[0].x + m_planes[0].y * m_planes[0].y + m_planes[0].z * m_planes[0].z);

	m_planes[0] *= t;

	m_planes[1].x = m30 + m00;
	m_planes[1].y = m31 + m01;
	m_planes[1].z = m32 + m02;
	m_planes[1].w = m33 + m03;

	t = glm::fastInverseSqrt(m_planes[1].x * m_planes[1].x + m_planes[1].y * m_planes[1].y + m_planes[1].z * m_planes[1].z);

	m_planes[1] *= t;

	m_planes[2].x = m30 - m10;
	m_planes[2].y = m31 - m11;
	m_planes[2].z = m32 - m12;
	m_planes[2].w = m33 - m13;

	t = glm::fastInverseSqrt(m_planes[2].x * m_planes[2].x + m_planes[2].y * m_planes[2].y + m_planes[2].z * m_planes[2].z);

	m_planes[2] *= t;

	m_planes[3].x = m30 + m10;
	m_planes[3].y = m31 + m11;
	m_planes[3].z = m32 + m12;
	m_planes[3].w = m33 + m13;

	t = glm::fastInverseSqrt(m_planes[3].x * m_planes[3].x + m_planes[3].y * m_planes[3].y + m_planes[3].z * m_planes[3].z);

	m_planes[3] *= t;

	m_planes[4].x = m30 - m20;
	m_planes[4].y = m31 - m21;
	m_planes[4].z = m32 - m22;
	m_planes[4].w = m33 - m23;

	t = glm::fastInverseSqrt(m_planes[4].x * m_planes[4].x + m_planes[4].y * m_planes[4].y + m_planes[4].z * m_planes[4].z);

	m_planes[4] *= t;

	m_planes[5].x = m30 + m20;
	m_planes[5].y = m31 + m21;
	m_planes[5].z = m32 + m22;
	m_planes[5].w = m33 + m23;

	t = glm::fastInverseSqrt(m_planes[5].x * m_planes[5].x + m_planes[5].y * m_planes[5].y + m_planes[5].z * m_planes[5].z);

	m_planes[5] *= t;
}

bool Frustum::pointInFrustum(const glm::vec3& a_point) const
{
	for (int p = 0; p < 6; p++)
		if (m_planes[p].x * a_point.x + m_planes[p].y * a_point.y + m_planes[p].z * a_point.z + m_planes[p].w <= 0)
			return false;

	return true;
}

bool Frustum::sphereInFrustum(const glm::vec3& a_point, float a_radius) const
{
	for (int p = 0; p < 6; p++)
		if (m_planes[p].x * a_point.x + m_planes[p].y * a_point.y + m_planes[p].z * a_point.z + m_planes[p].w + a_radius <= 0)
			return false;

	return true;
}

bool Frustum::aabbInFrustum(const glm::vec3& a_center, const glm::vec3& a_extent, const glm::mat4& a_frustumMatrix)
{
	const glm::vec4& rowX = a_frustumMatrix[0];
	const glm::vec4& rowY = a_frustumMatrix[1];
	const glm::vec4& rowZ = a_frustumMatrix[2];
	const glm::vec4& rowW = a_frustumMatrix[3];

	// Left and right planes              
	if (extentSignedTest(rowW + rowX, a_center, a_extent))
		return false;

	if (extentSignedTest(rowW - rowX, a_center, a_extent))
		return false;

	// Bottom and top planes
	if (extentSignedTest(rowW + rowY, a_center, a_extent))
		return false;

	if (extentSignedTest(rowW - rowY, a_center, a_extent))
		return false;

	// Near and far planes
	if (extentSignedTest(rowW + rowZ, a_center, a_extent))
		return false;

	if (extentSignedTest(rowW - rowZ, a_center, a_extent))
		return false;

	return true;
}

Frustum::Corners Frustum::calculateCorners(const PerspectiveCamera& a_camera)
{
	const float near = a_camera.getNear();
	const float far = a_camera.getFar();
	const float fovV = a_camera.getVFov();
	const float ratio = a_camera.getHeight() / a_camera.getWidth();
	const glm::vec3& pos = a_camera.getPosition();
	const glm::vec3& front = a_camera.getDirection();
	const glm::vec3& up = a_camera.getUp();

	const float tang = glm::tan(fovV * 0.5f);
	const float nh = near * tang;
	const float nw = nh * ratio;
	const float fh = far  * tang;
	const float fw = fh * ratio;

	const glm::vec3 right = glm::cross(up, front);
	const glm::vec3 nc = pos + front * near;
	const glm::vec3 fc = pos + front * far;

	Corners corners;
	corners.corners[Corners::NTL] = nc + up * nh - right * nw;
	corners.corners[Corners::NTR] = nc + up * nh + right * nw;
	corners.corners[Corners::NBL] = nc - up * nh - right * nw;
	corners.corners[Corners::NBR] = nc - up * nh + right * nw;

	corners.corners[Corners::FTL] = fc + up * fh - right * fw;
	corners.corners[Corners::FTR] = fc + up * fh + right * fw;
	corners.corners[Corners::FBL] = fc - up * fh - right * fw;
	corners.corners[Corners::FBR] = fc - up * fh + right * fw;

	return corners;
}