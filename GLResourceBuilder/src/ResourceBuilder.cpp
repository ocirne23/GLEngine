#include "ResourceBuilder.h"

#include "Utils/listFiles.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <Windows.h>

namespace {

static const char* BUILT_FILE_EXTENSION = ".da";

typedef std::string FileName;
typedef std::string FileTime;

typedef std::unordered_map<FileName, FileWriteTime*> StoredTimesMap;
typedef std::unordered_map<FileName, FileTime> CurrentTimesMap;
typedef std::unordered_map<std::string, ResourceProcessor*> ResourceProcessorMap;

struct Dependency
{
	FileName fileName;
	FileTime fileTime;
};

struct FileWriteTime
{
	FileName fileName;
	FileTime fileTime;
	std::vector<Dependency> dependencies;
};

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

void writeModificationFileToMap(std::fstream& a_file, StoredTimesMap& a_storedTimesMap)
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
			fileWriteTime->fileTime = line.substr(split + 1);
			a_storedTimesMap.insert({ fileWriteTime->fileName, fileWriteTime });
			prev = fileWriteTime;
		}
	}
}

void clearModificationFile(std::fstream& a_file)
{
	assert(a_file.is_open());
	a_file.clear();
}

void getAllFilesAndModificationTimesInFolder(CurrentTimesMap& a_outFiles, const std::string& a_folder)
{
	assert(!a_outFiles.size());
	std::vector<std::string> files;
	std::vector<std::string> writeTimes;
	listFiles(a_folder, "*", files, writeTimes);
	assert(files.size() == writeTimes.size());
	for (int i = 0; i < files.size(); ++i)
		a_outFiles.insert({files[i], writeTimes[i]});
}

void writeModificationMapToFile(std::fstream& a_file, const StoredTimesMap& a_writeTimesMap)
{
	// TODO: assert that a_file is empty?
	for (const auto& filePathAndWriteTime : a_writeTimesMap)
	{
		const std::string line = filePathAndWriteTime.first + ":" + filePathAndWriteTime.second->fileTime + "\n";
		a_file.write(line.c_str(), line.length());
		for (const auto& dependency : filePathAndWriteTime.second->dependencies)
		{
			const std::string dep = std::string(">") + dependency.fileName + ":" + dependency.fileTime + "\n";
			a_file.write(dep.c_str(), dep.length());
		}
		delete filePathAndWriteTime.second;
	}
}

bool isFileModified(const FileName& a_fileName, const FileTime& a_fileTime,
	const StoredTimesMap& a_storedTimesMap)
{
	const auto storedTimesIt = a_storedTimesMap.find(a_fileName);
	return (storedTimesIt != a_storedTimesMap.end() &&
		a_fileTime != storedTimesIt->second->fileTime)
}

bool isDependencyModified(const std::vector<Dependency>& a_dependencies, 
	const StoredTimesMap& a_writeTimesMap)
{
	for (const Dependency& dep : a_dependencies)
	{
		if (isFileModified(dep.fileName, dep.fileTime, a_writeTimesMap))
			return true;
	}
	return false;
}

ResourceProcessor* getResourceProcessorForFile(const FileName& a_filePath, const ResourceProcessorMap& a_processors)
{
	const std::string extension = getExtensionForFilePath(a_filePath);
	const auto processorsIt = a_processors.find(extension);
	if (processorsIt != a_processors.end())
		return processorsIt->second;
	else
		return NULL;
}

void processFileAndUpdateStoredTimes(const FileName& a_inPath, 
	                                 const FileName& a_outPath,
									 ResourceProcessor* a_processor, 
									 const StoredTimesMap& a_storedTimesMap)
{
	std::vector<FileName> dependencies;
	a_processor->process(a_inPath.c_str(), a_outPath.c_str(), dependencies);
}

} // namespace

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Starting resource building in folder: \t %s \n", a_inDirectoryPath);

	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);
	const std::string modificationFilePath = outDirectoryPathStr + "\\modificationdates.txt";

	StoredTimesMap storedTimesMap;
	CurrentTimesMap currentTimesMap;
	std::fstream modificationFile;

	if (modificationFileExists(modificationFilePath))
	{
		modificationFile.open(modificationFilePath, std::ios::in | std::ios::out);
		writeModificationFileToMap(modificationFile, storedTimesMap);
		clearModificationFile(modificationFile);
	}
	else
	{
		createDirectoryForFile(modificationFilePath);
		modificationFile.open(modificationFilePath, std::ios::in | std::ios::out);
	}

	getAllFilesAndModificationTimesInFolder(currentTimesMap, inDirectoryPathStr);
	for (const auto& currNameTime : currentTimesMap)
	{
		const FileName& fileName = currNameTime.first;
		const FileTime& fileTime = currNameTime.second;
		ResourceProcessor* processor = getResourceProcessorForFile(fileName, a_processors);
		if (processor)
		{
			if (a_incremental || isFileModified(fileName, fileTime, storedTimesMap) || isDependencyModified())
			{
				processFile(processor, currentTimesMap, storedTimesMap);
			}
		}
	}

	processChangedFilesAndUpdateModificationDates(a_processors, currentTimesMap, storedTimesMap, a_incremental);
	writeModificationMapToFile(modificationFile, storedTimesMap);

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

					auto at = storedTimesMap.find(str);
					if (at != storedTimesMap.end())
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
						storedTimesMap.insert({ str, fileWriteTime });
					}
				}
				else
				{
					printf("failed %s\n", str.c_str());
				}
			}
		}
	}

	for (const auto& oldWriteTimes : storedTimesMap)
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
