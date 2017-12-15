#pragma once

#include <sol/sol.hpp>
#include "ECS/PhysicsSystem.h"

class PhysicsBindings
{
public:

	static CreateBodyMessage createBodyMessage(sol::table table);

private:
};