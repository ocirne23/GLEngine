#pragma once

#include <glm/glm.hpp>

class Plane
{
public:

	enum Side
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
		return distance < 0.0f ? Plane::NEGATIVE : Plane::POSITIVE;
	}

	Side getSide(const glm::vec3& a_centre, const glm::vec3& a_halfSize) const
	{
		float dist = getDistance(a_centre);
		float maxAbsDist = glm::abs(normal.x * a_halfSize.x) + glm::abs(normal.y * a_halfSize.y) + glm::abs(normal.z * a_halfSize.z);
		if (dist < -maxAbsDist)
			return NEGATIVE;
		if (dist > +maxAbsDist)
			return POSITIVE;
		return BOTH;
	}

public:

	glm::vec3 normal;
	float d;
};