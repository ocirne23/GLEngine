#pragma once

#include "Core.h"
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
	static eastl::string getFileExtension(const eastl::string& filePath);
	static eastl::string getFolderPathForFile(const eastl::string& filePath);
	/** Includes extention */
	static eastl::string getFileNameFromPath(const eastl::string& filePath);
	/**year:month:day:hour:minute:second:millisecond */
	static eastl::string getFileTime(const eastl::string& filePath); // TODO: add structure storing time in non string form
	static eastl::vector<eastl::string> listFiles(const eastl::string& path, const eastl::string& mask);
	static eastl::string getExtensionForFilePath(const eastl::string& path);
	static void createDirectoryForFile(const eastl::string& filePath);
	static bool fileExists(const eastl::string& filePath);
	static eastl::string getApplicationExePath();

private:
	FileUtils() {}
	~FileUtils() {}
};

