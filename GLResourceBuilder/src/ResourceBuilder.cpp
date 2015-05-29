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

struct Dependency
{
	std::string fileName;
	std::string writeTimeStr;
};

struct FileWriteTime
{
	std::string fileName;
	std::string writeTimeStr;
	std::vector<Dependency> dependencies;
};

bool modificationFileExists(const std::string& a_filePath)
{
	bool exists = false;
	std::fstream file;
	file.open(a_filePath, std::fstream::in); // prolly not the right way to check for exists
	exists = file.is_open();
	if (exists)
		file.close();
	return exists;
}

std::fstream createModificationFile(const std::string& a_filePath)
{
	std::fstream file;
	file.open(a_filePath, std::fstream::in | std::fstream::out);
	assert(file.is_open());
	return file;
}

void writeModificationFileToMap(std::fstream& a_file, std::unordered_map<std::string, FileWriteTime*>& a_writeTimesMap)
{
	// TODO: assert that line iterator is at start of file
	FileWriteTime* prev = NULL;
	std::string line;
	while (std::getline(a_file, line))
	{
		const std::string::size_type split = line.find_first_of(":");
		if (line[0] == '>')
		{
			prev->dependencies.push_back({ line.substr(1, split), line.substr(split + 1) });
		}
		else
		{
			FileWriteTime* fileWriteTime = new FileWriteTime();
			fileWriteTime->fileName = line.substr(0, split);
			fileWriteTime->writeTimeStr = line.substr(split + 1);
			a_writeTimesMap.insert({ fileWriteTime->fileName, fileWriteTime });
			prev = fileWriteTime;
		}
	}
}

void clearModificationFile(std::fstream& a_file)
{
	assert(a_file.is_open());
	a_file.clear();
}

void getAllFilesAndModificationTimesInFolder(std::vector<std::pair<std::string, std::string>>& a_outFiles, const std::string& a_folder)
{
	assert(!a_outFiles.size());
	std::vector<std::string> files;
	std::vector<std::string> writeTimes;
	listFiles(a_folder, "*", files, writeTimes);
	assert(files.size() == writeTimes.size());
	for (int i = 0; i < files.size(); ++i)
		a_outFiles.push_back({files[i], writeTimes[i]});
}

void writeModificationMapToFile(std::fstream& a_file, std::unordered_map<std::string, FileWriteTime*>& a_writeTimesMap)
{
	// TODO: assert that a_file is empty?
	for (const auto& filePathAndWriteTime : a_writeTimesMap)
	{
		const std::string line = filePathAndWriteTime.first + ":" + filePathAndWriteTime.second->writeTimeStr + "\n";
		a_file.write(line.c_str(), line.length());
		for (const auto& dependency : filePathAndWriteTime.second->dependencies)
		{
			const std::string dep = std::string(">") + dependency.fileName + ":" + dependency.writeTimeStr + "\n";
			a_file.write(dep.c_str(), dep.length());
		}
		delete filePathAndWriteTime.second;
	}
}

bool checkForFileModified(const std::pair<std::string, std::string>& a_fileAndWriteTime)
{
	return false;
}

void processChangedFilesAndUpdateModificationDates(const std::unordered_map<std::string, ResourceProcessor*>& a_processors,
                                                   const std::vector<std::pair<std::string, std::string>>& a_fileWriteTimes,
                                                   std::unordered_map<std::string, FileWriteTime*>& a_writeTimesMap,
                                                   bool a_incremental)
{
	const int numFiles = (int) a_fileWriteTimes.size();
#pragma omp parallel for
	for (int i = 0; i < numFiles; ++i)
	{
		const std::string extension = getExtensionForFilePath(a_fileWriteTimes[i].first);
		const auto processorsIt = a_processors.find(extension);
		if (processorsIt != a_processors.end())
		{	// if a processor exists for this extension, process.
			if (!a_incremental || (a_incremental && checkForFileModified(a_fileWriteTimes[i]))
			{
				//processorsIt->second->process()
			}
		}
	}
}

} // namespace

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Starting resource building in folder: \t %s \n", a_inDirectoryPath);

	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);
	const std::string modificationFilePath = outDirectoryPathStr + "\\modificationdates.txt";

	std::unordered_map<std::string, FileWriteTime*> writeTimesMap;
	std::fstream modificationFile;

	if (modificationFileExists(modificationFilePath))
	{
		modificationFile.open(modificationFilePath, std::ios::in | std::ios::out);
		writeModificationFileToMap(modificationFile, writeTimesMap);
		clearModificationFile();
	}
	else
	{
		createDirectoryForFile(modificationFilePath);
		modificationFile.open(modificationFilePath, std::ios::in | std::ios::out);
	}

	std::vector<std::pair<std::string, std::string>> fileWriteTimes;
	getAllFilesAndModificationTimesInFolder(fileWriteTimes, inDirectoryPathStr);
	processChangedFilesAndUpdateModificationDates(a_processors, fileWriteTimes, writeTimesMap, a_incremental);
	
	writeModificationMapToFile(modificationFile, writeTimesMap);



	std::unordered_map<std::string, FileWriteTime*> oldWriteTimesMap;
	FileWriteTime* prev = NULL;
	std::string line;
	while (std::getline(modificationFile, line))
	{
		const std::string::size_type split = line.find_first_of(":");
		if (line[0] == '>')
		{
			prev->dependencies.push_back({line.substr(1, split), line.substr(split + 1)});
		}
		else
		{
			FileWriteTime* fileWriteTime = new FileWriteTime();
			fileWriteTime->fileName = line.substr(0, split);
			fileWriteTime->writeTimeStr = line.substr(split + 1);
			oldWriteTimesMap.insert({fileWriteTime->fileName, fileWriteTime});
			prev = fileWriteTime;
		}
	}
	modificationFile.clear();

	std::vector<std::string> files;
	std::vector<std::string> writeTimes;
	listFiles(a_inDirectoryPath, "*", files, writeTimes);

	const int numFiles = (int) files.size();
#pragma omp parallel for
	for (int i = 0; i < numFiles; ++i)
	{
		const std::string& str = files[i];
		const std::string& writeTime = writeTimes[i];

		if (a_incremental)
		{
			auto writeTimesIt = oldWriteTimesMap.find(str);
			//if (writeTimesIt != oldWriteTimesMap.end() && writeTimesIt->second->writeTimeStr == writeTime) // check if file was modified
			//	continue;
		}

		const std::string extension = getExtensionForFilePath(str);
		const auto processorsIt = a_processors.find(extension);
		if (processorsIt != a_processors.end())
		{	// if a processor exists for this extension, process.
			const std::string inFilePath = inDirectoryPathStr + "\\" + str;
			const std::string outFilePath = outDirectoryPathStr + "\\" + str.substr(0, str.find_last_of('.')) + BUILT_FILE_EXTENSION;
			const std::string outDirectoryPath = outFilePath.substr(0, outFilePath.find_last_of("\\"));

			createDirectoryForFile(outFilePath);

			printf("starting %s\n", str.c_str());
			std::vector<std::string> rebuildOnFileModificationList;
			if (processorsIt->second->process(inFilePath.c_str(), outFilePath.c_str(), rebuildOnFileModificationList))
			{
				const std::string line = str + ":" + writeTime + "\n";
				modificationFile.write(line.c_str(), line.length());
				auto at = oldWriteTimesMap.find(str);
				if (at != oldWriteTimesMap.end())
					oldWriteTimesMap.erase(at);
			}
			else
			{
				printf("failed %s\n", str.c_str());
			}

			const std::string extension = getExtensionForFilePath(str);
			const auto processorsIt = a_processors.find(extension);
			if (processorsIt != a_processors.end())
			{	// if a processor exists for this extension, process.
				const std::string inFilePath = inDirectoryPathStr + "\\" + str;
				const std::string outFilePath = outDirectoryPathStr + "\\" + str.substr(0, str.find_last_of('.')) + BUILT_FILE_EXTENSION;
				const std::string outDirectoryPath = outFilePath.substr(0, outFilePath.find_last_of("\\"));

				createDirectoryForFile(outFilePath);

				printf("starting %s\n", str.c_str());
				std::vector<std::string> rebuildDependencies;
				if (processorsIt->second->process(inFilePath.c_str(), outFilePath.c_str(), rebuildDependencies))
				{
					printf("finished %s\n", str.c_str());

					auto at = writeTimesMap.find(str);
					if (at != writeTimesMap.end())
					{
						at->second->writeTimeStr = writeTime;
					}
					else
					{
						FileWriteTime* fileWriteTime = new FileWriteTime();
						fileWriteTime->fileName = str;
						fileWriteTime->writeTimeStr = writeTime;
						for (const std::string& depStr : rebuildDependencies)
						{
							Dependency dependency;
							dependency.fileName = depStr;
							for (int i = 0; i < files.size(); ++i)
							{
								if (depStr == files[i])
								{
									dependency.writeTimeStr = writeTimes[i];
									break;
								}
							}
							fileWriteTime->dependencies.push_back(dependency);
						}
						writeTimesMap.insert({ str, fileWriteTime });
					}
				}
				else
				{
					printf("failed %s\n", str.c_str());
				}
			}
		}
	}

	for (const auto& oldWriteTimes : writeTimesMap)
	{
		const std::string line = oldWriteTimes.first + ":" + oldWriteTimes.second->writeTimeStr + "\n";
		modificationFile.write(line.c_str(), line.length());
		for (const auto& dependency : oldWriteTimes.second->dependencies)
		{
			const std::string dep = std::string(">") + dependency.fileName + ":" + dependency.writeTimeStr + "\n";
			modificationFile.write(dep.c_str(), dep.length());
		}
		delete oldWriteTimes.second;
	}
	modificationFile.close();

	printf("Done building resources in folder: \t %s \n", a_inDirectoryPath);
}

void ResourceBuilder::copyFiles(const std::vector<std::string>& a_extensions, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath, a_outDirectoryPath);
	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);

	std::vector<std::string> files;
	std::vector<std::string> lastWriteTimes;
	if (listFiles(a_inDirectoryPath, "*", files, lastWriteTimes))
	{
		for (const auto& filePath : files)
		{
			const std::string extension = getExtensionForFilePath(filePath);
			if (std::find(a_extensions.begin(), a_extensions.end(), extension) != a_extensions.end())
			{
				const std::string from = inDirectoryPathStr + "\\" + filePath;
				const std::string to = outDirectoryPathStr + "\\" + filePath;
				createDirectoryForFile(to);
				CopyFile(from.c_str(), to.c_str(), false);
			}
		}
	}
	printf("Finished copying files.\n");
}
