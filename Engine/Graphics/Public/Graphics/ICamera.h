#pragma once

#include "GraphicsAPI.h"
#include "Core/PimplPtr.h"
#include <glm/fwd.hpp>

class Frustum;
class Camera;

class GRAPHICS_API ICamera
{
public:
	enum class EProjection { PERSPECTIVE, ORTHOGRAPHIC };

	ICamera();
	explicit operator Camera&() { return *m_impl; }

	void initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp);
	void updateMatrices();

	void setPosition(const glm::vec3& position);
	void setDirection(const glm::vec3& direction);
	void setHorizontalFov(float hFov);
	void setWorldUp(const glm::vec3& worldUp);
	void lookAtPoint(const glm::vec3& position);

	void translateAbsolute(const glm::vec3& translation);
	void translateRelative(const glm::vec3& translation);
	void rotateAbsolute(float angle, const glm::vec3& axis);
	void rotateRelative(float xRot, float yRot);

	float getRotationRadXY() const;
	float getRotationRadXZ() const;
	float getRotationRadYZ() const;

	const Frustum& getFrustum()	const;
	const glm::mat4& getCombinedMatrix() const;
	const glm::mat4& getProjectionMatrix() const;
	const glm::mat4& getViewMatrix() const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getDirection() const;
	const glm::vec3& getUp() const;
	float getWidth() const;
	float getHeight() const;
	float getVFov() const;
	float getHFov() const;
	float getNear() const;
	float getFar() const;

private:

	PimplPtr<Camera> m_impl;
};