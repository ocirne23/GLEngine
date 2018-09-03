#pragma once

#include "Graphics/ICamera.h"
#include "Frustum.h"

class Camera : public ICamera
{
public:

	virtual void initialize(float width, float height, float horizontalFov, float near, float far, EProjection projection, const glm::vec3& worldUp) override;
	virtual void updateMatrices() override;

	virtual void setPosition(const glm::vec3& position) override;
	virtual void setDirection(const glm::vec3& direction) override;
	virtual void setHorizontalFov(float hFov) override;
	virtual void setWorldUp(const glm::vec3& worldUp) override;
	virtual void lookAtPoint(const glm::vec3& position) override;

	virtual void translateAbsolute(const glm::vec3& translation) override;
	virtual void translateRelative(const glm::vec3& translation) override;
	virtual void rotateAbsolute(float angle, const glm::vec3& axis) override;
	virtual void rotateRelative(float xRot, float yRot) override;

	virtual const Frustum& getFrustum() const override            { return m_frustum; }
	virtual float getRotationRadXY() const override               { return std::atan2(m_direction.x, m_direction.y); }
	virtual float getRotationRadXZ() const override               { return std::atan2(m_direction.x, m_direction.z); }
	virtual float getRotationRadYZ() const override               { return std::atan2(m_direction.y, m_direction.z); }
	virtual const glm::mat4& getCombinedMatrix() const override   { return m_combinedMatrix; }
	virtual const glm::mat4& getProjectionMatrix() const override { return m_projectionMatrix; }
	virtual const glm::mat4& getViewMatrix() const override       { return m_viewMatrix; }
	virtual const glm::vec3& getPosition() const override         { return m_position; }
	virtual const glm::vec3& getDirection() const override        { return m_direction; }
	virtual const glm::vec3& getUp() const override               { return m_up; }
	virtual float getWidth() const override                       { return m_width; }
	virtual float getHeight() const override                      { return m_height; }
	virtual float getVFov() const override                        { return m_vFieldOfView; }
	virtual float getHFov() const override                        { return m_hFieldOfView; }
	virtual float getNear() const override                        { return m_near; }
	virtual float getFar() const override                         { return m_far; }

private:

	friend class Graphics;
	virtual ~Camera() override {}

private:

	EProjection m_projection;
	Frustum m_frustum;

	glm::mat4 m_combinedMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_direction = glm::vec3(0, 0, -1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);
	glm::vec3 m_worldUp = glm::vec3(0, 1, 0);

	float m_width = 0.0f;
	float m_height = 0.0f;
	float m_near = 0.0f;
	float m_far = 0.0f;
	float m_hFieldOfView = 0.0f;
	float m_vFieldOfView = 0.0f;
};