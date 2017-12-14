#include "Overworld.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Utils/StringUtils.h"

#include <Box2D/Box2D.h>
#include <Box2D/Common/b2Draw.h>

BEGIN_UNNAMED_NAMESPACE()

const float TIMESTEP = 1 / 20.0f;
const int POSITION_ITERATIONS = 3;
const int VELOCITY_ITERATIONS = 8;

END_UNNAMED_NAMESPACE()

Overworld::Overworld()
{
	m_physicsWorld = new b2World({ 0, 0 });
	m_box2dDebugDraw.initialize();
	m_box2dDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
	m_physicsWorld->SetDebugDraw(&m_box2dDebugDraw);
	m_camera.initialize(scast<float>(GLEngine::graphics->getViewportWidth()), scast<float>(GLEngine::graphics->getViewportHeight()), 90.0f, 0.1f, 1000.0f);
	m_cameraController.setCameraSpeed(5.0f);
	m_camera.setPosition({ 0, 0, 10 });
	m_camera.lookAtPoint({ 0, 0, 0 });

	m_fpsMeasurer.setLogFunction(1.0f, [this]() {
		GLEngine::graphics->setWindowTitle(("GPBoatGame FPS: " + StringUtils::to_string(m_fpsMeasurer.getAvgFps())).c_str());
	});

	b2BodyDef bodyDef;
	b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
	b2CircleShape shape;
	shape.m_radius = 3.0f;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	b2Fixture* fixture = body->CreateFixture(&fixtureDef);
	body->SetActive(true);
}

Overworld::~Overworld()
{
	SAFE_DELETE(m_physicsWorld);
}

void Overworld::update(float a_deltaSec)
{
	if (GLEngine::input->isKeyPressed(EKey::ESCAPE))
		GLEngine::shutdown();

	m_fpsMeasurer.tickFrame(a_deltaSec);

	GLEngine::graphics->clear({ 0.15, 0.15, 0.15, 1.0 });

	m_cameraController.update(m_camera, a_deltaSec, true);

	m_timestepAccumulator += a_deltaSec;
	if (m_timestepAccumulator > a_deltaSec)
	{
		m_timestepAccumulator -= a_deltaSec;
		m_physicsWorld->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	}

	m_physicsWorld->DrawDebugData();
	m_box2dDebugDraw.render(m_camera);

	GLEngine::graphics->swap();
}