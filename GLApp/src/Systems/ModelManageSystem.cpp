#include "Systems/ModelManageSystem.h"

#include "3rdparty/rde/rde_string.h"
#include "Components/ModelComponent.h"
#include "Graphics/GL/GLMesh.h"
#include "Systems/RenderSystem.h"

ModelManageSystem::ModelManageSystem()
{

}

ModelManageSystem::~ModelManageSystem()
{
	for (auto pair : m_meshes)
		delete pair.second;
}

void ModelManageSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
}

void ModelManageSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<ModelComponent>>(*this);
}

void ModelManageSystem::receive(entityx::ComponentAddedEvent<ModelComponent>& a_modelComponentAddedEvent)
{
	auto& component = a_modelComponentAddedEvent.component;

	auto it = m_meshes.find(component->filePath);
	GLMesh* mesh = NULL;
	if (it == m_meshes.end())
	{
		mesh = new GLMesh();	
		mesh->loadFromFile(component->filePath.c_str(), TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);
		m_meshes.insert({component->filePath, mesh});
	}
	else
	{
		mesh = it->second;
	}
	component->mesh = mesh;
}

