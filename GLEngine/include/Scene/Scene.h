#pragma once

#include "Core.h"
#include "rde/vector.h"

class Actor;
class System;

class Scene
{
public:
	Scene() : m_lastID(0) {}
	~Scene() {}
	Scene(const Scene& copy) = delete;

	void update(float deltaSec);
	void render();

	void addSystem(System* system);
	void removeSystem(System* system);

	Actor* createActor();
	void addActor(Actor* actor);
	void deleteActor(Actor* actor);

private:

	uint m_lastID;
	rde::vector<System*> m_systems;
};