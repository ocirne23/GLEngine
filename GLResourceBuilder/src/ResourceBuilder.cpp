#include "ResourceBuilder.h"

#include "Utils\listFiles.h"
#include <Windows.h>
#include <assert.h>
#include <fstream>
#include <algorithm>

static const char* BUILT_FILE_EXTENSION = ".da";

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& processors, const char* inDirectoryPath, const char* outDirectoryPath, bool incremental)
{
	printf("Starting resource building in folder: \t %s \n", inDirectoryPath);

	std::unordered_map<std::string, std::string> oldWriteTimesMap;
	std::fstream writeTimesFile;

	writeTimesFile.open(std::string(outDirectoryPath) + "\\modificationdates.txt", std::ios::in);
	if (writeTimesFile.is_open())
	{	// Read modification file
		std::string line;
		while (std::getline(writeTimesFile, line))
		{
			std::string fileName;
			std::string writeTimeStr;
			std::string::size_type split = line.find_first_of(":");
			fileName = line.substr(0, split);
			writeTimeStr = line.substr(split + 1);
			oldWriteTimesMap.insert(std::make_pair(fileName, writeTimeStr));
		}
	}
	writeTimesFile.close();

	std::vector<std::string> files;
	std::vector<std::string> writeTimes;

	std::string outDirectoryPathString(outDirectoryPath);
	{	// Create out directory if not exists
		std::string::size_type from = 0;
		while (from != std::string::npos)
		{
			from = outDirectoryPathString.find_first_of("\\", from + 1);
			std::string makeDirPath = outDirectoryPathString.substr(0, from);
			CreateDirectory(makeDirPath.c_str(), NULL);
		}
	}

	writeTimesFile.open(std::string(outDirectoryPath) + "\\modificationdates.txt", std::ios::out);
	assert(writeTimesFile.is_open());
	writeTimesFile.clear();

	if (listFiles(inDirectoryPath, "*", files, writeTimes))
	{
		int count = (int) files.size();
#pragma omp parallel for
		for (int i = 0; i < count; ++i)
		{
			const std::string& str = files[i];
			const std::string& writeTime = writeTimes[i];

			if (incremental)
			{
				auto writeTimesIt = oldWriteTimesMap.find(str);
				if (writeTimesIt != oldWriteTimesMap.end())
				{	// check if file was modified
					if (writeTimesIt->second == writeTime)
					{
						continue;
					}
				}
			}

			std::string::size_type extIdx = str.find_last_of('.');
			if (extIdx == std::string::npos)
			{	// only process files with an extension
				continue; 
			}

			std::string inDirectoryPathStr(inDirectoryPath);
			std::string outDirectoryPathStr(outDirectoryPath);
			std::string extension = str.substr(extIdx + 1, str.length() - extIdx - 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

			auto it = processors.find(extension);
			if (it != processors.end())
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
				if (it->second->process(inFilePath.c_str(), outFilePath.c_str()))
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

	printf("Done building resources in folder: \t %s \n", inDirectoryPath);
}