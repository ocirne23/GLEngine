#pragma once

#include "3rdparty/entityx/System.h"
#include "3rdparty/rde/hash_map.h"
#include "3rdparty/rde/rde_string.h"

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

	rde::hash_map<rde::string, GLMesh*> m_meshes;
};
