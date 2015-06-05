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

typedef std::unordered_map<FileName, FileWriteTime*> StoredTimesMap;
typedef std::unordered_map<FileName, FileTime> CurrentTimesMap;
typedef std::unordered_map<std::string, ResourceProcessor*> ResourceProcessorMap;

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
		a_fileTime != storedTimesIt->second->fileTime);
}

bool isDependencyModified(const FileName& a_fileName, const StoredTimesMap& a_storedTimesMap, const CurrentTimesMap& a_currentTimesMap)
{
	auto it = a_storedTimesMap.find(a_fileName);
	if (it != a_storedTimesMap.end())
		return true;
	auto dependencies = it->second->dependencies;
	for (const Dependency& dep : dependencies)
	{
		if (isFileModified(dep.fileName, dep.fileTime, a_storedTimesMap))
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

void processFileAndUpdateStoredTimes(const std::string& a_inPath, 
	                                 const std::string& a_outPath,
									 ResourceProcessor* a_processor, 
									 StoredTimesMap& a_storedTimesMap)
{
	printf("Processing: %s \n", a_inPath.c_str());
	std::vector<FileName> dependencies;
	a_processor->process(a_inPath.c_str(), a_outPath.c_str(), dependencies);
	printf("done\n");
}

} // namespace

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Building files in folder: %s\n", a_inDirectoryPath);

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
			if (!a_incremental || isFileModified(fileName, fileTime, storedTimesMap) || isDependencyModified(fileName, storedTimesMap, currentTimesMap))
			{
				const std::string inPath = inDirectoryPathStr + '\\' + fileName;
				const std::string outPath = outDirectoryPathStr + '\\' + fileName + BUILT_FILE_EXTENSION;
				createDirectoryForFile(outPath);
				processFileAndUpdateStoredTimes(inPath, outPath, processor, storedTimesMap);
			}
		}
	}
	writeModificationMapToFile(modificationFile, storedTimesMap);
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
