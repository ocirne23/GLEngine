#include "listFiles.h"

#include <Windows.h>
#include <stack>
#include <codecvt>
#include <assert.h>

bool listFiles(std::string a_path, std::string a_mask, std::vector<std::string>& a_files, std::vector<std::string>& a_lastWriteTimesStr)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	std::string spec;
	std::stack<std::string> directories;
	std::string root = a_path;

	directories.push(a_path);
	a_files.clear();

	while (!directories.empty())
	{
		a_path = directories.top();
		spec = a_path + "\\" + a_mask;
		directories.pop();
		const char* da = "wa";
		hFind = FindFirstFile(spec.c_str(), &ffd);

		if (hFind == INVALID_HANDLE_VALUE)
			return false;

		do
		{
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					directories.push(a_path + "\\" + ffd.cFileName);
				}
				else
				{
					const std::string fullPath = a_path + "\\" + ffd.cFileName;
					a_files.push_back(fullPath.substr(root.length() + 1, fullPath.length() - (root.length() + 1)));

					SYSTEMTIME sysWriteTime;
					FileTimeToSystemTime(&ffd.ftLastWriteTime, &sysWriteTime);

					std::string time;
					time += std::to_string((int) sysWriteTime.wHour) + ":";
					time += std::to_string((int) sysWriteTime.wMinute) + ":";
					time += std::to_string((int) sysWriteTime.wSecond) + ":";
					time += std::to_string((int) sysWriteTime.wMilliseconds) + "-";
					time += std::to_string((int) sysWriteTime.wDay) + ":";
					time += std::to_string((int) sysWriteTime.wMonth) + ":";
					time += std::to_string((int) sysWriteTime.wYear);

					a_lastWriteTimesStr.push_back(time);
				}
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			return false;
		}

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}