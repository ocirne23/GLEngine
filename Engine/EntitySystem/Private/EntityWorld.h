#pragma once

#include "EntitySystem/IEntityWorld.h"

class EntityWorld : public IEntityWorld
{
public:

private:

	friend class EntitySystem;
	virtual ~EntityWorld() override {}
};