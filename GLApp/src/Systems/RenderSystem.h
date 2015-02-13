#pragma once

#include "Core.h"

#include "entityx/System.h"

#include "Graphics/LightManager.h"
#include "Graphics/GL/Core/GLConstantBuffer.h"
#include "Graphics/GL/Core/GLShader.h"
#include "Graphics/GL/Core/GLTexture.h"
#include "Graphics/GL/Core/GLTextureBuffer.h"
#include "Graphics/GL/Core/GLUniform.h"
#include "Graphics/GL/Tech/ClusteredShading.h"

#include <glm/glm.hpp>

struct CameraComponent;
class PerspectiveCamera;
class LightSystem;

class RenderSystem : public entityx::System<RenderSystem>, public entityx::Receiver<RenderSystem>
{
public:
	RenderSystem(LightSystem& lightSystem);
	virtual ~RenderSystem() OVERRIDE;

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void configure(entityx::EventManager& eventManager);

	const GLShader& getModelShader() { return m_modelShader; }

private:

	void initializeShaderForCamera(const PerspectiveCamera& camera);

private:

	LightSystem& m_lightSystem;

	const PerspectiveCamera* m_activeCamera = NULL;

	GLTexture m_dfvTexture;
	GLShader m_modelShader;

	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;

	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;

	GLUniform<glm::mat3> m_normalMatrixUniform;
	GLUniform<glm::mat4> m_viewMatrixUniform;
	GLUniform<glm::mat4> m_mvpMatrixUniform;
	GLUniform<glm::mat4> m_transformUniform;

};