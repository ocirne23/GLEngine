#include "Scene/Scene.h"

#include "Scene/System.h"
#include "Scene/Actor.h"

Scene::Scene() : m_lastID(0)
{}

void Scene::addSystem(System* system)
{
	m_systems.push_back(system);
}

void Scene::removeSystem(System* system)
{
	m_systems.erase(rde::find(m_systems.begin(), m_systems.end(), system));
}

Actor* Scene::createActor()
{
	Actor* actor = new Actor();
	actor->m_id = ++m_lastID; // start at 1
	return actor;
}