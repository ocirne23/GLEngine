#include "ResourceBuilder.h"

#include "Utils/listFiles.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <Windows.h>

static const char* BUILT_FILE_EXTENSION = ".da";

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Starting resource building in folder: \t %s \n", a_inDirectoryPath);

	std::unordered_map<std::string, std::string> oldWriteTimesMap;
	std::fstream writeTimesFile;

	writeTimesFile.open(std::string(a_outDirectoryPath) + "\\modificationdates.txt", std::ios::in);
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
	}
	writeTimesFile.close();

	std::vector<std::string> files;
	std::vector<std::string> writeTimes;

	const std::string outDirectoryPathString(a_outDirectoryPath);
	// Create out directory if not exists
	std::string::size_type pathSubFolderIt = 0;
	while (pathSubFolderIt != std::string::npos)
	{
		pathSubFolderIt = outDirectoryPathString.find_first_of("\\", pathSubFolderIt + 1);
		std::string makeDirPath = outDirectoryPathString.substr(0, pathSubFolderIt);
		CreateDirectory(makeDirPath.c_str(), NULL);
	}

	writeTimesFile.open(std::string(a_outDirectoryPath) + "\\modificationdates.txt", std::ios::out);
	assert(writeTimesFile.is_open());
	writeTimesFile.clear();

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

			std::string::size_type extIdx = str.find_last_of('.');
			if (extIdx == std::string::npos) // only process files with an extension
				continue; 

			std::string inDirectoryPathStr(a_inDirectoryPath);
			std::string outDirectoryPathStr(a_outDirectoryPath);
			std::string extension = str.substr(extIdx + 1, str.length() - extIdx - 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

			const auto processorsIt = a_processors.find(extension);
			if (processorsIt != a_processors.end())
			{	// if a processor exists for this extension, process.
				std::string inFilePath = inDirectoryPathStr + "\\" + str;
				std::string outFilePath = outDirectoryPathStr + "\\" + str.substr(0, extIdx) + BUILT_FILE_EXTENSION;
				std::string outDirectoryFilePath = outFilePath.substr(0, outFilePath.find_last_of("\\"));

				std::string::size_type from = outDirectoryPathString.length();
				while (from != std::string::npos)
				{	// create sub directories for file if not exists
					from = outDirectoryFilePath.find_first_of("\\", from + 1);
					std::string makeDirPath = outDirectoryFilePath.substr(0, from);
					CreateDirectory(makeDirPath.c_str(), NULL);
				}

				printf("starting %s\n", str.c_str());
				if (processorsIt->second->process(inFilePath.c_str(), outFilePath.c_str()))
				{
					printf("finished %s\n", str.c_str());
					std::string line = str + ":" + writeTime + "\n";
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
		std::string line = oldWriteTimes.first + ":" + oldWriteTimes.second + "\n";
		writeTimesFile.write(line.c_str(), line.length());
	}

	writeTimesFile.close();

	printf("Done building resources in folder: \t %s \n", a_inDirectoryPath);
}