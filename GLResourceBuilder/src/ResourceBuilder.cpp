#include "ResourceBuilder.h"

#include "Utils/FileUtils.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()

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

void ResourceBuilder::buildResourcesDB(const ResourceProcessorMap& a_processors, const std::string& a_inDirectoryPath, AssetDatabase& a_assetDatabase)
{
	for (const std::string& filePath : FileUtils::listFiles(a_inDirectoryPath, "*"))
	{
		ResourceProcessor* processor = getResourceProcessorForFile(filePath, a_processors);
		if (processor)
		{
			print("Processing: %s\n", filePath.c_str());
			processor->process(filePath, a_assetDatabase);
			print("Finished processing %s\n", filePath.c_str());
		}
	}
}

void ResourceBuilder::copyFiles(const std::vector<std::string>& a_extensions, const std::string& a_inDirectoryPath, const std::string& a_outDirectoryPath)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath, a_outDirectoryPath);
	for (const std::string& filePath : FileUtils::listFiles(a_inDirectoryPath, "*"))
	{
		const std::string extension = FileUtils::getExtensionForFilePath(filePath);
		if (std::find(a_extensions.begin(), a_extensions.end(), extension) != a_extensions.end())
		{
			const std::string from = a_inDirectoryPath + "\\" + filePath;
			const std::string to = a_outDirectoryPath + "\\" + filePath;
			FileUtils::createDirectoryForFile(to);
			CopyFile(from.c_str(), to.c_str(), false);
		}
	}
	printf("Finished copying files.\n");
}