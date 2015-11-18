#pragma once

#include "BuilderCore.h"

#include <string>
#include <vector>

struct FileTime
{
	uint dwLowDateTime  = 0;
	uint dwHighDateTime = 0;
};

class FileUtils
{
public:
	static FileTime getCurrentFileTime();

	static std::string getFileExtension(const std::string& a_filePath);

	static std::string getFolderPathForFile(const std::string& a_filePath);

	static std::string getFileTime(const std::string& filePath);

	static bool listFiles(std::string a_path, std::string a_mask, std::vector<std::string>& a_files);

	static std::string getExtensionForFilePath(const std::string& a_path);

	static void createDirectoryForFile(const std::string& a_filePath);

	static bool fileExists(const std::string& a_filePath);

private:
	FileUtils() {}
	~FileUtils() {}
};

