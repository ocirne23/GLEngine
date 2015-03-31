#include "ResourceBuilder.h"

#include "Utils/listFiles.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <Windows.h>

namespace {

static const char* BUILT_FILE_EXTENSION = ".da";

std::string getExtensionForFilePath(const std::string& a_path)
{
	const std::string::size_type extIdx = a_path.find_last_of('.');
	if (extIdx == std::string::npos)
		return "";
	std::string extension = a_path.substr(extIdx + 1, a_path.length() - extIdx - 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
	return extension;
}

void createDirectoryForFile(const std::string& a_filePath)
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

} // namespace

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Starting resource building in folder: \t %s \n", a_inDirectoryPath);

	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);
	const std::string modificationFilePath = outDirectoryPathStr + "\\modificationdates.txt";

	std::unordered_map<std::string, std::string> oldWriteTimesMap;
	std::fstream writeTimesFile;

	createDirectoryForFile(modificationFilePath);
	writeTimesFile.open(modificationFilePath, std::ios::in);
	if (writeTimesFile.is_open())
	{	// Read modification file
		std::string line;
		while (std::getline(writeTimesFile, line))
		{
			const std::string::size_type split = line.find_first_of(":");
			const std::string fileName = line.substr(0, split);
			const std::string writeTimeStr = line.substr(split + 1);
			oldWriteTimesMap.insert(std::make_pair(fileName, writeTimeStr));
		}
		writeTimesFile.clear();
		writeTimesFile.close();
	}
	writeTimesFile.open(modificationFilePath, std::ios::out);
	assert(writeTimesFile.is_open());

	std::vector<std::string> files;
	std::vector<std::string> writeTimes;
	if (listFiles(a_inDirectoryPath, "*", files, writeTimes))
	{
		const int count = (int) files.size();
#pragma omp parallel for
		for (int i = 0; i < count; ++i)
		{
			const std::string& str = files[i];
			const std::string& writeTime = writeTimes[i];

			if (a_incremental)
			{
				auto writeTimesIt = oldWriteTimesMap.find(str);
				if (writeTimesIt != oldWriteTimesMap.end() && writeTimesIt->second == writeTime) // check if file was modified
					continue;
			}

			const std::string extension = getExtensionForFilePath(str);
			const auto processorsIt = a_processors.find(extension);
			if (processorsIt != a_processors.end())
			{	// if a processor exists for this extension, process.
				const std::string inFilePath = inDirectoryPathStr + "\\" + str;
				const std::string outFilePath = outDirectoryPathStr + "\\" + str.substr(0, str.find_last_of('.')) + BUILT_FILE_EXTENSION;
				const std::string outDirectoryFilePath = outFilePath.substr(0, outFilePath.find_last_of("\\"));

				createDirectoryForFile(outFilePath);

				printf("starting %s\n", str.c_str());
				if (processorsIt->second->process(inFilePath.c_str(), outFilePath.c_str()))
				{
					printf("finished %s\n", str.c_str());

					const std::string line = str + ":" + writeTime + "\n";
					writeTimesFile.write(line.c_str(), line.length());
					auto at = oldWriteTimesMap.find(str);
					if (at != oldWriteTimesMap.end())
						oldWriteTimesMap.erase(at);
				}
				else
				{
					printf("failed %s\n", str.c_str());
				}
			}
		}
	}

	for (const auto& oldWriteTimes : oldWriteTimesMap)
	{
		const std::string line = oldWriteTimes.first + ":" + oldWriteTimes.second + "\n";
		writeTimesFile.write(line.c_str(), line.length());
	}
	writeTimesFile.close();

	printf("Done building resources in folder: \t %s \n", a_inDirectoryPath);
}

void ResourceBuilder::copyFiles(const std::vector<std::string>& a_extensions, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath, a_outDirectoryPath);
	std::string inDirectoryPathStr(a_inDirectoryPath);
	std::string outDirectoryPathStr(a_outDirectoryPath);

	std::vector<std::string> files;
	std::vector<std::string> lastWriteTimes;
	if (listFiles(a_inDirectoryPath, "*", files, lastWriteTimes))
	{
		for (const auto& filePath : files)
		{
			std::string extension = getExtensionForFilePath(filePath);
			if (std::find(a_extensions.begin(), a_extensions.end(), extension) != a_extensions.end())
			{
				std::string from = inDirectoryPathStr + "\\" + filePath;
				std::string to = outDirectoryPathStr + "\\" + filePath;
				createDirectoryForFile(to);
				CopyFile(from.c_str(), to.c_str(), false);
			}
		}
	}
	printf("Finished copying files.\n");
}
