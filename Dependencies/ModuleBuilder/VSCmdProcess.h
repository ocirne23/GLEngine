#pragma once

#include <Windows.h>
#include <vector>
#include <string>

class VSCmdProcess
{
public:

	VSCmdProcess();
	~VSCmdProcess();

	//std::vector<char> readFromConsole();

	void writeToConsole(const std::string& str);
	void changeDirectory(const std::string& dir);

private:

	HANDLE stdinRd = nullptr;
	HANDLE stdinWr = nullptr;
	PROCESS_INFORMATION processInfo = {};
};