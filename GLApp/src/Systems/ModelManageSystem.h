#pragma once

#include "entityx/System.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"

struct ModelComponent;
class GLMesh;

class ModelManageSystem : public entityx::System<ModelManageSystem>, public entityx::Receiver<ModelManageSystem>
{
public:
	ModelManageSystem();
	~ModelManageSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(entityx::ComponentAddedEvent<ModelComponent>& modelComponentAddedEvent);

private:

	eastl::hash_map<eastl::string, GLMesh*> m_meshes;
};
