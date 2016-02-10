#include "Database/ResourceBuilder.h"

#include "Utils/FileUtils.h"
#include "EASTL/algorithm.h"

#define NOMINMAX
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()

ResourceProcessor* getResourceProcessorForFile(const eastl::string& a_filePath, const ResourceBuilder::ResourceProcessorMap& a_processors)
{
	const eastl::string extension = FileUtils::getExtensionForFilePath(a_filePath);
	const auto processorsIt = a_processors.find(extension);
	if (processorsIt != a_processors.end())
		return processorsIt->second;
	else
		return NULL;
}

END_UNNAMED_NAMESPACE()

void ResourceBuilder::buildResourcesDB(const ResourceProcessorMap& a_processors, const eastl::string& a_inDirectoryPath, AssetDatabase& a_assetDatabase)
{
	for (const eastl::string& filePath : FileUtils::listFiles(a_inDirectoryPath, "*"))
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

void ResourceBuilder::copyFiles(const eastl::vector<eastl::string>& a_extensions, const eastl::string& a_inDirectoryPath, const eastl::string& a_outDirectoryPath)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath.c_str(), a_outDirectoryPath.c_str());
	for (const eastl::string& filePath : FileUtils::listFiles(a_inDirectoryPath, "*"))
	{
		const eastl::string extension = FileUtils::getExtensionForFilePath(filePath);
		if (eastl::find(a_extensions.begin(), a_extensions.end(), extension) != a_extensions.end())
		{
			const eastl::string from = a_inDirectoryPath + "\\" + filePath;
			const eastl::string to = a_outDirectoryPath + "\\" + filePath;
			FileUtils::createDirectoryForFile(to);
			CopyFile(from.c_str(), to.c_str(), false);
		}
	}
	printf("Finished copying files.\n");
}