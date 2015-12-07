#include "Utils/FileUtils.h"

#include "EASTL/heap.h"
#include "EASTL/string.h"
#include "EASTL/to_string.h"

#include <algorithm>
#include <assert.h>
#include <codecvt>
#include <fstream>
#include <stack>
#define NOMINMAX
#include <Windows.h>

eastl::string FileUtils::getFileExtension(const eastl::string& a_filePath)
{
	auto dotIdx = a_filePath.find_last_of('.');
	if (dotIdx != eastl::string::npos)
		return a_filePath.substr(dotIdx + 1);
	else
		return "";
}

eastl::string FileUtils::getFolderPathForFile(const eastl::string& a_filePath)
{
	auto dirIdx = std::min(a_filePath.find_last_of('/'), a_filePath.find_last_of('\\'));
	if (dirIdx != eastl::string::npos)
		return a_filePath.substr(0, dirIdx).append("\\");
	else
		return "";
}

eastl::string FileUtils::getFileNameFromPath(const eastl::string& a_filePath)
{
	auto dirIdx = std::min(a_filePath.find_last_of('/'), a_filePath.find_last_of('\\'));
	if (dirIdx != eastl::string::npos)
		return a_filePath.substr(dirIdx + 1, a_filePath.size());
	else
		return "";
}

eastl::string FileUtils::getFileTime(const eastl::string& a_filePath)
{
	LPCWSTR filePath = eastl::wstring(a_filePath.c_str()).c_str();

	WIN32_FILE_ATTRIBUTE_DATA fileAttrData = {0};
	GetFileAttributesExW(filePath, GetFileExInfoStandard, &fileAttrData);

	SYSTEMTIME sysWriteTime;
	FileTimeToSystemTime(&fileAttrData.ftLastWriteTime, &sysWriteTime);

	eastl::string time;

	time += StringUtils::to_string((int) sysWriteTime.wYear) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wMonth) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wDay) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wHour) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wMinute) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wSecond) + ":";
	time += StringUtils::to_string((int) sysWriteTime.wMilliseconds);

	assert(sysWriteTime.wYear != 1601);

	return time;
}

eastl::vector<eastl::string> FileUtils::listFiles(const eastl::string& a_path, const eastl::string& a_mask)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	eastl::vector<eastl::string> files;
	eastl::vector<eastl::string> directories;
	eastl::string path = a_path;
	directories.push_back(a_path);

	while (!directories.empty())
	{
		path = directories.back();
		directories.pop_back();

		eastl::string spec = path + "\\" + a_mask;
		hFind = FindFirstFile(spec.c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
			return files;
		do
		{
			if (strcmp(ffd.cFileName, ".") && strcmp(ffd.cFileName, ".."))
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // If directory, add to list of directories to search
					directories.push_back(path + "\\" + ffd.cFileName);
				else
				{
					const eastl::string fullPath = path + "\\" + ffd.cFileName;
					files.push_back(fullPath);
				}
			}
		}
		while (FindNextFile(hFind, &ffd));
		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			return files;
		}
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return files;
}

eastl::string FileUtils::getExtensionForFilePath(const eastl::string& a_path)
{
	const eastl::string::size_type extIdx = a_path.find_last_of('.');
	if (extIdx == eastl::string::npos)
		return "";
	eastl::string extension = a_path.substr(extIdx + 1, a_path.length() - extIdx - 1);
	extension.make_lower();
	return extension;
}

void FileUtils::createDirectoryForFile(const eastl::string& a_filePath)
{
	eastl::string directory = a_filePath.substr(0, a_filePath.find_last_of("\\"));

	eastl::string::size_type from = 0;
	while (from != eastl::string::npos)
	{	// create sub directories for file if not exists
		from = directory.find_first_of("\\", from + 1);
		const eastl::string makeDirPath = directory.substr(0, from);
		CreateDirectory(makeDirPath.c_str(), NULL);
	}
}

bool FileUtils::fileExists(const eastl::string& a_filePath)
{
	bool exists = false;
	std::fstream file;
	file.open(a_filePath.c_str(), std::fstream::in); // prolly not the right way to check for exists
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

