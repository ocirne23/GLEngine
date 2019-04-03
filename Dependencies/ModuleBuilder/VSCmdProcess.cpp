#include "VSCmdProcess.h"

#include <assert.h>

#include "CreatePipeEx.h"

VSCmdProcess::VSCmdProcess()
{
	SetLastError(0); // ._.

	DWORD lastError = GetLastError();
	if (lastError != ERROR_SUCCESS)
	{
		fprintf(stderr, "err %d\n", lastError);
		__debugbreak();
	}
	
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = true;

	if (!CreatePipeEx(&stdinRd, &stdinWr, &sa, 0, FILE_FLAG_OVERLAPPED, FILE_FLAG_OVERLAPPED))
	{
		printf("Pipe creation failed\n");
		exit(0xc0ffee);
	}

	lastError = GetLastError();
	if (lastError != ERROR_SUCCESS)
	{
		fprintf(stderr, "err %d\n", lastError);
		__debugbreak();
	}

	ZeroMemory(&processInfo, sizeof(processInfo));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;
	si.hStdInput = stdinRd;
	const char* exePath = R"(C:\WINDOWS\system32\cmd.exe)";
	const char* toolsDir = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\VC\Auxiliary\Build)";
	const char* cmdArgs = R"(/K "vcvars64.bat")";

	if (!CreateProcessA(exePath, (char*)cmdArgs, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, toolsDir, &si, &processInfo))
	{
		printf("Error Spawning Command Prompt.\n");
		__debugbreak();
	}
	SetLastError(0); // ._.

	lastError = GetLastError();
	if (lastError != ERROR_SUCCESS)
	{
		fprintf(stderr, "err %d\n", lastError);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		__debugbreak();
	}
}

void VSCmdProcess::changeDirectory(const std::string& dir)
{
	std::string cmd = "cd /d " + dir + "\n";
	writeToConsole(cmd);
}

VSCmdProcess::~VSCmdProcess()
{
	TerminateProcess(processInfo.hProcess, 0);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}

VOID WINAPI Thing(
	_In_    DWORD dwErrorCode,
	_In_    DWORD dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED lpOverlapped
)
{
	printf("%i", dwNumberOfBytesTransfered);
}

void VSCmdProcess::writeToConsole(const std::string& str)
{
	if (str.length() <= 1) return;
	assert(str[str.length() - 1] == '\n');
	OVERLAPPED overlapped = {};
	BOOL res = WriteFileEx(stdinWr, str.c_str(), (DWORD)str.length(), &overlapped, Thing);
	assert(res);
}
