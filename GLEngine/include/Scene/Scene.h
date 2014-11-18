#pragma once

class Actor;

class Scene
{
public:
	Scene();
	~Scene();
	Scene(const Scene& copy) = delete;

	void update(float deltaSec);
	void render();


private:

	Actor* createActor();
	void deleteActor(Actor* actor);

	
};