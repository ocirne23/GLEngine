#pragma once

#include <glm\glm.hpp>

class Light
{
public:
	Light();
	~Light();
	Light(const Light& copy) = delete;

private:
	glm::vec3 position;
	glm::vec3 color;
	float range;
};