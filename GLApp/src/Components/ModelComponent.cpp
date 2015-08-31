#include "Components/ModelComponent.h"

#include "Graphics/GL/GLMesh.h"

ModelComponent::~ModelComponent()
{
	SAFE_DELETE(mesh);
}