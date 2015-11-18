#include "Utils/FileUtils.h"

#include <algorithm>
#include <assert.h>
#include <codecvt>
#include <fstream>
#include <stack>
#define NOMINMAX
#include <Windows.h>

std::string FileUtils::getFileExtension(const std::string& a_filePath)
{
	auto dotIdx = a_filePath.find_last_of('.');
	if (dotIdx != std::string::npos)
		return a_filePath.substr(dotIdx + 1);
	else
		return "";
}

std::string FileUtils::getFolderPathForFile(const std::string& a_filePath)
{
	auto dirIdx = std::min(a_filePath.find_last_of('/'), a_filePath.find_last_of('\\'));
	if (dirIdx != std::string::npos)
		return a_filePath.substr(0, dirIdx).append("\\");
	else
		return "";
}

std::string FileUtils::getFileTime(const std::string& a_filePath)
{
	std::wstring stemp = std::wstring(a_filePath.begin(), a_filePath.end());
	LPCWSTR filePath = stemp.c_str();

	WIN32_FILE_ATTRIBUTE_DATA fileAttrData = {0};
	GetFileAttributesExW(filePath, GetFileExInfoStandard, &fileAttrData);

	SYSTEMTIME sysWriteTime;
	FileTimeToSystemTime(&fileAttrData.ftLastWriteTime, &sysWriteTime);

	std::string time;
	time += std::to_string((int) sysWriteTime.wYear) + ":";
	time += std::to_string((int) sysWriteTime.wMonth) + ":";
	time += std::to_string((int) sysWriteTime.wDay) + ":";
	time += std::to_string((int) sysWriteTime.wHour) + ":";
	time += std::to_string((int) sysWriteTime.wMinute) + ":";
	time += std::to_string((int) sysWriteTime.wSecond) + ":";
	time += std::to_string((int) sysWriteTime.wMilliseconds);

	assert(sysWriteTime.wYear != 1601);

	return time;
}

bool FileUtils::listFiles(std::string a_path, std::string a_mask, std::vector<std::string>& a_files)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	std::stack<std::string> directories;
	std::string root = a_path;

	directories.push(a_path);
	a_files.clear();

	while (!directories.empty())
	{
		a_path = directories.top();
		std::string spec = a_path + "\\" + a_mask;
		directories.pop();

		hFind = FindFirstFile(spec.c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
			return false;

		do
		{
			if (strcmp(ffd.cFileName, ".") && strcmp(ffd.cFileName, ".."))
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					directories.push(a_path + "\\" + ffd.cFileName);
				}
				else
				{
					const std::string fullPath = a_path + "\\" + ffd.cFileName;
					a_files.push_back(fullPath.substr(root.length() + 1, fullPath.length() - (root.length() + 1)));
				}
			}
		}
		while (FindNextFile(hFind, &ffd));

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

std::string FileUtils::getExtensionForFilePath(const std::string& a_path)
{
	const std::string::size_type extIdx = a_path.find_last_of('.');
	if (extIdx == std::string::npos)
		return "";
	std::string extension = a_path.substr(extIdx + 1, a_path.length() - extIdx - 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	return extension;
}

void FileUtils::createDirectoryForFile(const std::string& a_filePath)
{
	std::string directory = a_filePath.substr(0, a_filePath.find_last_of("\\"));

	std::string::size_type from = 0;
	while (from != std::string::npos)
	{	// create sub directories for file if not exists
		from = directory.find_first_of("\\", from + 1);
		const std::string makeDirPath = directory.substr(0, from);
		CreateDirectory(makeDirPath.c_str(), NULL);
	}
}

bool FileUtils::fileExists(const std::string& a_filePath)
{
	bool exists = false;
	std::fstream file;
	file.open(a_filePath, std::fstream::in); // prolly not the right way to check for exists
	exists = file.is_open();
	if (exists)
		file.close();
	return exists;
}

FileTime FileUtils::getCurrentFileTime()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	FileTime ft;
	SystemTimeToFileTime(&st, (_FILETIME*)(&ft));
	return ft;
}

