#include "ECS/LogicSystem.h"


#include "Utils/FileHandle.h"
#include "Utils/FileUtils.h"

BEGIN_UNNAMED_NAMESPACE()

auto loadLuaScript(sol::state& a_lua, const char* a_filePath)
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
{
	m_lua.open_libraries(sol::lib::base, sol::lib::package);
	const eastl::string luaFolderPath = getLuaFolder();
	eastl::string packagePath = luaFolderPath;
	packagePath.append("?.lua");
	m_lua["package"]["path"] = packagePath.c_str();
	loadLuaScript(m_lua, "assets/lua/folders.lua");
	sol::table folders = m_lua["folders"];
	for (auto item : folders)
	{
		packagePath.append(";;");
		packagePath.append(luaFolderPath);
		sol::string_view folder = item.second.as<sol::string_view>();
		packagePath.append(folder.data());
		packagePath.append("\\?.lua");
		print("%s\n", folder.data());
	}
	m_lua["package"]["path"] = packagePath.c_str();

	m_lua.set_function("print", [](sol::string_view str)
	{
		print("lua: %s\n", str.data());
	});

	loadLuaScript(m_lua, "assets/lua/startup.lua");
	loadLuaScript(m_lua, "assets/lua/input.lua");
	
	sol::function keydown = m_lua["input"]["keydown"];
	assert(keydown.valid());
	sol::function keyup = m_lua["input"]["keyup"];
	assert(keyup.valid());

	m_keyDownListener.setFunc([=](EKey a_key, bool a_isRepeat)
	{
		if (a_isRepeat)
			return;
		keydown(a_key);
		if (a_key == EKey::ESCAPE) 
			GLEngine::shutdown();
	});
	m_keyUpListener.setFunc([=](EKey a_key)
	{
		keyup(a_key);
	});
}

void LogicSystem::update(float a_deltaSec)
{
	GLEngine::processInput();
	sol::function update = m_lua["input"]["update"];
	update(a_deltaSec);
}
