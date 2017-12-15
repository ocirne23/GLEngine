#include "Logic/LuaEntityBindings.h"

#include "ECS/EntitySystem.h"

void LuaEntityBindings::initialize()
{
	sol::constructors<Entity(), void(ushort, ushort)> ctor;
	sol::usertype<Entity> utype(ctor, "index", &Entity::index, "version", &Entity::version);
	m_lua.set_usertype<Entity>(utype);

	m_lua.set_function("createEntity", [&]()
	{
		Entity e = m_entitySystem.createEntity();
	});
}
