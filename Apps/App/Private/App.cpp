#include "App.h"

App::App()
	: database(*m_engine.m_database)
	, filesystem(*m_engine.m_filesystem)
	, graphics(*m_engine.m_graphics)
	, input(*m_engine.m_input)
	, network(*m_engine.m_network)
	, threading(*m_engine.m_threading)
{
}