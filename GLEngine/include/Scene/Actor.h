#pragma once

#include "Core.h"
#include <glm/glm.hpp>

class Actor
{
public:
	friend class Scene;

	Actor(const Actor& copy) = delete;
private:

	Actor();
	~Actor();

private:

	uint m_id;
};