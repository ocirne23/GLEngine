#include "BoatGame.h"

#include "GLEngine.h"
#include "Utils/DeltaTimeMeasurer.h"
#include "ECS/Entity.h"

#include "ECS/EntitySystem.h"
#include "ECS/RenderSystem.h"
#include "ECS/LogicSystem.h"
#include "ECS/PhysicsSystem.h"

#include <sol/sol.hpp>

BoatGame::BoatGame()
{
	m_entitySystem = new EntitySystem();
	m_physicsSystem = new PhysicsSystem(*this);
	m_logicSystem = new LogicSystem(*this);

	GLEngine::createThread("RenderThread", [this]()
	{
		GLEngine::createGLContext();
		m_renderSystem = new RenderSystem(*this);
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			m_renderSystem->render(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
		}
		GLEngine::destroyGLContext();
	});

	GLEngine::createThread("PhysicsThread", [this]()
	{
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			m_physicsSystem->update(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
			GLEngine::sleep(1);
		}
	});

	GLEngine::createThread("LogicThread", [this]() 
	{
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			m_logicSystem->update(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
			GLEngine::sleep(1);
		}
	});

	GLEngine::createThread("FileThread", [this]()
	{
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			GLEngine::sleep(1);
		}
	});

	{
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			GLEngine::doMainThreadTick();
			GLEngine::sleep(1);
		}
	}
}

BoatGame::~BoatGame()
{
	GLEngine::waitForAllThreadShutdown();

	SAFE_DELETE(m_renderSystem);
	SAFE_DELETE(m_logicSystem);
	SAFE_DELETE(m_physicsSystem);
	SAFE_DELETE(m_entitySystem);
}
