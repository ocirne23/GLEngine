#include "ResourceBuilder.h"

#include "Utils/FileUtils.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <windows.h>
#include <omp.h>

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
	FileTime fileTime;
	std::vector<Dependency> dependencies;
};

typedef std::unordered_map<FileName, FileWriteTime*> ModificationFileMap;
typedef std::unordered_map<std::string, ResourceProcessor*> ResourceProcessorMap;

std::fstream createModificationFile(const std::string& a_filePath)
{
	std::fstream file;
	file.open(a_filePath, std::fstream::in | std::fstream::out);
	assert(file.is_open());
	return file;
}

void writeModificationFileToMap(std::fstream& a_file, ModificationFileMap& a_storedTimesMap)
{
	// TODO: assert that line iterator is at start of file
	FileWriteTime* prev = NULL;
	std::string line;
	while (std::getline(a_file, line))
	{
		const std::string::size_type split = line.find_first_of(":");
		if (line[0] == '>')
		{
			prev->dependencies.push_back({ line.substr(1, split - 1), line.substr(split + 1) });
		}
		else
		{
			FileName fileName = line.substr(0, split);
			FileWriteTime* fileWriteTime = new FileWriteTime();
			fileWriteTime->fileTime = line.substr(split + 1);
			a_storedTimesMap.insert({ fileName, fileWriteTime });
			prev = fileWriteTime;
		}
	}
}

std::vector<std::string> getAllFilesInFolder(const std::string& a_folder)
{
	std::vector<std::string> files;
	listFiles(a_folder, "*", files);
	return files;
}

void writeModificationMapToFile(const std::string& a_filePath, const ModificationFileMap& a_writeTimesMap)
{
	std::ofstream modificationFile(a_filePath);
	assert(modificationFile.is_open());
	// TODO: assert that a_file is empty?
	for (const auto& filePathAndWriteTime : a_writeTimesMap)
	{
		const std::string line = filePathAndWriteTime.first + ":" + filePathAndWriteTime.second->fileTime + "\n";
		modificationFile.write(line.c_str(), line.length());
		for (const auto& dependency : filePathAndWriteTime.second->dependencies)
		{
			const std::string dep = std::string(">") + dependency.fileName + ":" + dependency.fileTime + "\n";
			modificationFile.write(dep.c_str(), dep.length());
		}
		delete filePathAndWriteTime.second;
	}
	modificationFile.close();
}

bool isFileOrDependencyModified(const FileName& a_fileName, const std::string& a_directoryPath,
	const ModificationFileMap& a_storedTimesMap)
{
	std::string fullPath = a_directoryPath + "\\" + a_fileName;
	FileTime fileTime = getFileTime(fullPath);

	auto it = a_storedTimesMap.find(a_fileName);
	if (it == a_storedTimesMap.end())
		return true;
	
	FileWriteTime* fileWriteTime = it->second;
	if (fileTime != fileWriteTime->fileTime)
		return true;

	for (const Dependency& dep : fileWriteTime->dependencies)
	{
		std::string fullDepPath = a_directoryPath + "\\" + getFolderPathForFile(a_fileName) + dep.fileName;
		FileTime depFileTime = getFileTime(fullDepPath);
		if (depFileTime != dep.fileTime)
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

FileWriteTime* processFile(const std::string& a_inPath, 
	             const std::string& a_outPath,
                 ResourceProcessor* a_processor)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("Processing: %s\n", a_inPath.c_str());
	std::vector<FileName> dependencies;
	a_processor->process(a_inPath.c_str(), a_outPath.c_str(), dependencies);
	printf("Finished: %s\n", a_inPath.c_str());

	FileWriteTime* fileWriteTime = new FileWriteTime();
	fileWriteTime->fileTime = getFileTime(a_inPath.c_str());
	for (const FileName& dependency : dependencies)
	{
		std::string fullDependencyPath = getFolderPathForFile(a_inPath) + dependency;
		fileWriteTime->dependencies.push_back({dependency, getFileTime(fullDependencyPath)});
	}

	return fileWriteTime;
}

} // namespace

void ResourceBuilder::buildResources(const std::unordered_map<std::string, ResourceProcessor*>& a_processors, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);
	const std::string modificationFilePath = outDirectoryPathStr + "\\modificationdates.txt";

	ModificationFileMap storedTimesMap;

	if (fileExists(modificationFilePath))
	{
		std::fstream modificationFile(modificationFilePath);
		writeModificationFileToMap(modificationFile, storedTimesMap);
		modificationFile.clear();
		modificationFile.close();
	}
	else
	{
		createDirectoryForFile(modificationFilePath);
		std::ofstream modificationFile(modificationFilePath);
		modificationFile.flush();
		modificationFile.close();
	}

	auto fileNames = getAllFilesInFolder(inDirectoryPathStr);
	const int numFiles = (int) fileNames.size();
omp_set_num_threads(20);
#pragma omp parallel for
	for (int i = 0; i < numFiles; ++i)
	{
		const FileName& fileName = fileNames[i];

		ResourceProcessor* processor = getResourceProcessorForFile(fileName, a_processors);
		if (processor)
		{
			const std::string inPath = inDirectoryPathStr + '\\' + fileName;
			const std::string outPath = outDirectoryPathStr + '\\' + fileName + BUILT_FILE_EXTENSION;

			if (!a_incremental || isFileOrDependencyModified(fileName, inDirectoryPathStr, storedTimesMap))
			{
				createDirectoryForFile(outPath);
				FileWriteTime* fileWriteTime = processFile(inPath, outPath, processor);
				storedTimesMap[fileName] = fileWriteTime;
			}
		}
	}
	writeModificationMapToFile(modificationFilePath, storedTimesMap);
}

void ResourceBuilder::copyFiles(const std::vector<std::string>& a_extensions, const char* a_inDirectoryPath, const char* a_outDirectoryPath, bool a_incremental)
{
	printf("Copying files from %s to %s\n", a_inDirectoryPath, a_outDirectoryPath);
	const std::string inDirectoryPathStr(a_inDirectoryPath);
	const std::string outDirectoryPathStr(a_outDirectoryPath);

	std::vector<std::string> files;
	if (listFiles(a_inDirectoryPath, "*", files))
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
