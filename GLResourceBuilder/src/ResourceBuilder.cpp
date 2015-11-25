#include "ResourceBuilder.h"

#include "Utils/FileUtils.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()

std::vector<std::string> getAllFilesInFolder(const std::string& a_folder)
{
	std::vector<std::string> files;
	FileUtils::listFiles(a_folder, "*", files);
	return files;
}

ResourceProcessor* getResourceProcessorForFile(const std::string& a_filePath, const ResourceBuilder::ResourceProcessorMap& a_processors)
{
	const std::string extension = FileUtils::getExtensionForFilePath(a_filePath);
	const auto processorsIt = a_processors.find(extension);
	if (processorsIt != a_processors.end())
		return processorsIt->second;
	else
		return NULL;
}

END_UNNAMED_NAMESPACE()

void ResourceBuilder::buildResourcesDB(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, AssetDatabase& a_assetDatabase, bool a_incremental)
{
	const std::string inDirectoryPathStr(a_inDirectoryPath);
	std::vector<std::string> fileList = getAllFilesInFolder(inDirectoryPathStr);
	for (auto filePath : fileList)
	{
		ResourceProcessor* processor = getResourceProcessorForFile(filePath, a_processors);
		if (processor)
		{
			std::vector<std::string> temp;
			print("Processing: %s\n", filePath.c_str());
			processor->process(filePath, a_assetDatabase, temp);
			print("Finished processing %s\n", filePath.c_str());
		}
	}
}

void ResourceBuilder::copyFiles(const std::vector<std::string>& a_extensions, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath, a_outDirectoryPath);
	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);

	std::vector<std::string> files;
	if (FileUtils::listFiles(a_inDirectoryPath, "*", files))
	{
		for (const auto& filePath : files)
		{
			const std::string extension = FileUtils::getExtensionForFilePath(filePath);
			if (std::find(a_extensions.begin(), a_extensions.end(), extension) != a_extensions.end())
			{
				const std::string from = inDirectoryPathStr + "\\" + filePath;
				const std::string to = outDirectoryPathStr + "\\" + filePath;
				FileUtils::createDirectoryForFile(to);
				CopyFile(from.c_str(), to.c_str(), false);
			}
		}
	}
	printf("Finished copying files.\n");
}