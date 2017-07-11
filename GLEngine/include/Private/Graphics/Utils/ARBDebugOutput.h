#pragma once

#include "Core.h"

class ARBDebugOutput
{
public:

	static void tryEnable();
	static void setDebugLog(bool a_enabled) { s_debugLogEnabled = a_enabled; }

	static uint s_numLogs;

private:

	ARBDebugOutput() {}
	static bool s_debugLogEnabled;
};

