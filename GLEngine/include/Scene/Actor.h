#pragma once

#include "Core.h"
#include <glm/glm.hpp>

class Actor
{
public:
	Actor();
	~Actor();
	Actor(const Actor& copy) = delete;
private:

	uint64 m_uuid;
	uint m_id;
	
	ushort* m_systemIDs;
	ushort m_numSystems;
};