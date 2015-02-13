#pragma once

class PerspectiveCamera;

struct CameraComponent
{
	CameraComponent() : camera(0) {}
	CameraComponent(PerspectiveCamera* camera) : camera(camera) {}

	PerspectiveCamera* camera;
};