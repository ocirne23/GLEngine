#pragma once

#include "Engine.h"

class Database;
class Filesystem;
class Graphics;
class Input;
class Network;
class Threading;

class App
{
public:

	App();

public:

	Database& database;
	Filesystem& filesystem;
	Graphics& graphics;
	Input& input;
	Network& network;
	Threading& threading;

private:

	Engine m_engine;
};