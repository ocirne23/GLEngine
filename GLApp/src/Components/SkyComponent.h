#pragma once

class GLMesh;

struct SkyComponent
{
	SkyComponent() {}

	bool centerOnCamera = true;
	glm::mat4 transform = glm::mat4(1);
};