#pragma once

#include "Graphics/Utils/PerspectiveCamera.h"

struct CameraComponent
{
	CameraComponent(float viewportWidth, float viewportHeight, float horizontalFov, float near, float far) 
	{
		camera.initialize(viewportWidth, viewportHeight, horizontalFov, near, far);
	}
	PerspectiveCamera camera;
};