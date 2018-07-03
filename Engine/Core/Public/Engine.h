#pragma once

#include "Core.h"

class Database;
class Filesystem;
class Graphics;
class Input;
class Network;
class Threading;

class Engine
{
public:

	Engine();
	Engine(Engine&) = delete;
	~Engine();

public:

	owner<Database*> m_database;
	owner<Filesystem*> m_filesystem;
	owner<Graphics*> m_graphics;
	owner<Input*> m_input;
	owner<Network*> m_network;
	owner<Threading*> m_threading;
};