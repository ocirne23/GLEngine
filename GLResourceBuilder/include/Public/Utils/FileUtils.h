#pragma once

#include "BuilderCore.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

struct FileTime
{
	uint dwLowDateTime  = 0;
	uint dwHighDateTime = 0;
};

class FileUtils
{
public:

	static FileTime getCurrentFileTime();
	static eastl::string getFileExtension(const eastl::string& a_filePath);
	static eastl::string getFolderPathForFile(const eastl::string& a_filePath);
	static eastl::string getFileTime(const eastl::string& filePath);
	static eastl::vector<eastl::string> listFiles(const eastl::string& a_path, const eastl::string& a_mask);
	static eastl::string getExtensionForFilePath(const eastl::string& a_path);
	static void createDirectoryForFile(const eastl::string& a_filePath);
	static bool fileExists(const eastl::string& a_filePath);

private:
	FileUtils() {}
	~FileUtils() {}
};

