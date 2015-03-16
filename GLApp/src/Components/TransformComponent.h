#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct TransformComponent
{
    TransformComponent() {}
    TransformComponent(const glm::mat4& transform) : transform(transform) {}
    TransformComponent(float x, float y, float z)
    {
	setPosition(glm::vec3(x, y, z));
    }

    TransformComponent(const glm::vec3& pos)
    {
	setPosition(pos);
    }

    void setPosition(const glm::vec3& pos)
    {
	transform[3] = glm::vec4(pos, 1.0f);
    }

    void translate(const glm::vec3& translation)
    {
	transform[3].x += translation.x;
	transform[3].y += translation.y;
	transform[3].z += translation.z;
    }

    void rotate(const glm::vec3& axis, float angle)
    {
	transform *= glm::rotate(angle, axis);
    }

    void scale(const glm::vec3& scale)
    {
	transform *= glm::scale(scale);
    }

    void setRotation(const glm::vec3& axis, float angle)
    {
	glm::vec4 pos = transform[3];
	transform = glm::rotate(angle, axis);
	transform[3] = pos;
    }

    glm::mat4 transform = glm::mat4(1);
};