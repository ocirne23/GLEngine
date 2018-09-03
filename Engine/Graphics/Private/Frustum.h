#pragma once

#include "Core.h"
#include <glm/glm.hpp>

class PerspectiveCamera;

class Frustum
{
public:

	Frustum() {}
	~Frustum() {}
	Frustum(const Frustum& copy) = delete;

	void calculateFrustum(const glm::mat4& vpMatrix);
	bool pointInFrustum(const glm::vec3& point) const;
	bool sphereInFrustum(const glm::vec3& point, float radius) const;
	bool aabbInFrustum(const glm::vec3& min, const glm::vec3& max) const;

private:

	class Plane
	{
	public:

		enum class Side
		{
			POSITIVE, NEGATIVE, BOTH
		};

	public:

		float getDistance(const glm::vec3& a_point) const
		{
			return glm::dot(normal, a_point) + d;
		}

		Side getSide(const glm::vec3& a_point) const
		{
			float distance = getDistance(a_point);
			return distance < 0.0f ? Side::NEGATIVE : Side::POSITIVE;
		}

		Side getSide(const glm::vec3& a_centre, const glm::vec3& a_halfSize) const
		{
			float dist = getDistance(a_centre);
			float maxAbsDist = glm::abs(normal.x * a_halfSize.x) + glm::abs(normal.y * a_halfSize.y) + glm::abs(normal.z * a_halfSize.z);
			if (dist < -maxAbsDist)
				return Side::NEGATIVE;
			if (dist > +maxAbsDist)
				return Side::POSITIVE;
			return Side::BOTH;
		}

	public:

		glm::vec3 normal;
		float d;
	};

private:

	Plane m_planes[6];
};