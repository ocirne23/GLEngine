#pragma once

#include "Core.h"

class IDatabase;
class IFilesystem;
class IGraphics;
class IInput;
class INetwork;
class IThreading;

class Engine
{
public:
	owner<IDatabase*> m_database;
	owner<IFilesystem*> m_filesystem;
	owner<IGraphics*> m_graphics;
	owner<IInput*> m_input;
	owner<INetwork*> m_network;
	owner<IThreading*> m_threading;
};