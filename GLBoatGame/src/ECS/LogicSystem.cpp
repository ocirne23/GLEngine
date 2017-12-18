#include "ECS/LogicSystem.h"


#include "Utils/FileHandle.h"
#include "Utils/FileUtils.h"
#include "Logic/LuaPhysicsBindings.h"
#include "BoatGame.h"
#include "ECS/PhysicsSystem.h"

#include <Box2D/Box2D.h>

BEGIN_UNNAMED_NAMESPACE()

eastl::string getLuaFolder()
{
	eastl::string path = FileUtils::getApplicationExePath();
	path = path.substr(0, path.rfind("bin\\"));
	path.append("assets\\lua\\");
	return path;
}

END_UNNAMED_NAMESPACE()

LogicSystem::LogicSystem(BoatGame& a_boatGame)
	: m_boatGame(a_boatGame)
	, m_entityBindings(m_lua, a_boatGame.getEntitySystem())
	, m_physicsBindings(m_lua, a_boatGame.getPhysicsSystem(), *this)
	, m_inputBindings(m_lua, *this)
{
	initializeLuaState();
	m_entityBindings.initialize();
	m_physicsBindings.initialize();
	m_inputBindings.initialize();

	m_keyDownListener.setFunc([&](EKey a_key, bool a_isRepeat)
	{
		if (a_isRepeat)
			return;
		m_inputBindings.keyDown(a_key);
		if (a_key == EKey::ESCAPE)
			GLEngine::shutdown();
	});
	m_keyUpListener.setFunc([&](EKey a_key)
	{
		m_inputBindings.keyUp(a_key);
	});

	loadLuaScript(m_lua, "assets/lua/startup.lua");
}

sol::protected_function_result LogicSystem::loadLuaScript(sol::state& a_lua, const char * a_filePath)
{
	const eastl::string script = FileHandle(a_filePath).readString();
	auto result = a_lua.safe_script(script.c_str());
	if (!result.valid())
	{
		sol::error err = result;
		auto status = result.status();
		print("loadLuaScript error: %s: %s\n", err.what(), sol::to_string(status));
	}
	assert(result.valid());
	return result;
}

void LogicSystem::initializeLuaState()
{
	m_lua.open_libraries(sol::lib::base, sol::lib::package);
	const eastl::string luaFolderPath = getLuaFolder();
	eastl::string packagePath = luaFolderPath;
	packagePath.append("?.lua");
	m_lua["package"]["path"] = packagePath.c_str();
	loadLuaScript(m_lua, "assets/lua/folders.lua");
	sol::table folders = m_lua["folders"];
	m_lua.script(R"(
		function bind(t, k)
			return function(...) return t[k](t, ...) end
		end
	)");


	for (auto item : folders)
	{
		packagePath.append(";;");
		packagePath.append(luaFolderPath);
		sol::string_view folder = item.second.as<sol::string_view>();
		packagePath.append(folder.data());
		packagePath.append("\\?.lua");
	}
	m_lua["package"]["path"] = packagePath.c_str();
	m_lua.set_function("print", [](sol::string_view str)
	{
		print("lua: %s\n", str.data());
	});
}

void LogicSystem::update(float a_deltaSec)
{
	GLEngine::processInput();
	m_inputBindings.update(a_deltaSec);
}

