#include "Engine.h"

#include "Database/Database.h"
#include "Filesystem/Filesystem.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Network/Network.h"
#include "Threading/Threading.h"

Engine::Engine()
{
	m_database = new Database();
	m_filesystem = new Filesystem();
	m_graphics = new Graphics();
	m_input = new Input();
	m_network = new Network();
	m_threading = new Threading();
}

Engine::~Engine()
{
	SAFE_DELETE(m_threading);
	SAFE_DELETE(m_network);
	SAFE_DELETE(m_input);
	SAFE_DELETE(m_graphics);
	SAFE_DELETE(m_filesystem);
	SAFE_DELETE(m_database);
}