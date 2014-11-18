#pragma once 

#include "Core.h"

#include "Utils\MatForward.h"
#include "Utils\VecForward.h"

#include "rde\vector.h"

class GLScene;
class GLMesh;
class PerspectiveCamera;

class Scene
{
public:

	Scene();
	~Scene();
	Scene(const Scene& copy) = delete;

	void addModel(const glm::mat4& transform, const GLMesh* mesh);
	void addLight(const glm::vec3& position, const glm::vec3& color, float range);
	void render(const PerspectiveCamera& camera);

private:

	struct Model
	{
		glm::mat4 transform;
		GLMesh* mesh;
	};

	struct Light
	{
		glm::vec3 position;
		glm::vec3 color;
		float range;
	};

	GLScene* m_glScene;
	rde::vector<Model> m_models;
	rde::vector<Light> m_lights;
};