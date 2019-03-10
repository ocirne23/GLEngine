#pragma once

#include "GraphicsAPI.h"

export module Graphics.ICamera;

class Frustum;

export class GRAPHICS_API ICamera
{
public:
	enum class EProjection { PERSPECTIVE, ORTHOGRAPHIC };
	/*

	virtual void initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp) = 0;
	virtual void updateMatrices() = 0;

	virtual void setPosition(const glm::vec3& position) = 0;
	virtual void setDirection(const glm::vec3& direction) = 0;
	virtual void setHorizontalFov(float hFov) = 0;
	virtual void setWorldUp(const glm::vec3& worldUp) = 0;
	virtual void lookAtPoint(const glm::vec3& position) = 0;

	virtual void translateAbsolute(const glm::vec3& translation) = 0;
	virtual void translateRelative(const glm::vec3& translation) = 0;
	virtual void rotateAbsolute(float angle, const glm::vec3& axis) = 0;
	virtual void rotateRelative(float xRot, float yRot) = 0;

	virtual float getRotationRadXY() const = 0;
	virtual float getRotationRadXZ() const = 0;
	virtual float getRotationRadYZ() const = 0;

	virtual const glm::mat4& getCombinedMatrix() const = 0;
	virtual const glm::mat4& getProjectionMatrix() const = 0;
	virtual const glm::mat4& getViewMatrix() const = 0;
	virtual const glm::vec3& getPosition() const = 0;
	virtual const glm::vec3& getDirection() const = 0;
	virtual const glm::vec3& getUp() const = 0;
	virtual float getWidth() const = 0;
	virtual float getHeight() const = 0;
	virtual float getVFov() const = 0;
	virtual float getHFov() const = 0;
	virtual float getNear() const = 0;
	virtual float getFar() const = 0;
	*/
};