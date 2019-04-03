#pragma once

#include <filesystem>
#include <functional>
#include <assert.h>
#include <iostream>

void listFilesWithExt(const std::filesystem::path& path, const std::string& ext, const std::function<void(const std::filesystem::directory_entry&)>& callback)
{
	std::filesystem::directory_iterator iter(path);
	for (auto i : iter)
	{
		if (i.is_directory())
		{
			listFilesWithExt(i.path(), ext, callback);
		}
		else if (i.is_regular_file() && i.path().extension().compare(ext) == 0)
		{
			callback(i);
		}
	}
}

std::pair<std::unique_ptr<char>, size_t> readFile(const std::string& filePath, size_t fileSize)
{
	FILE* file = nullptr;
	fopen_s(&file, filePath.c_str(), "r");
	if (file)
	{
		char* buf = (char*)malloc(fileSize);
		size_t numRead = 0;
		if (buf)
		{
			numRead = fread_s(buf, fileSize, 1, fileSize, file);
			buf = (char*)realloc(buf, numRead + 1);
			buf[numRead] = '\0';
			assert(feof(file));
		}
		fclose(file);
		return { std::unique_ptr<char>(buf), numRead + 1 };
	}
	return { std::unique_ptr<char>(), 0 };
}

//R"import[ \t]+(.+?(?=;))"
//R"((/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*[\r\n]))"

//std::regex removeCommentsRegex(R"((;.*[\r\n])|(--.*[\r\n]))");
//content = std::regex_replace(content, removeCommentsRegex, "");

std::string readUserInput()
{
	std::string str;
	std::getline(std::cin, str);
	str += "\n";
	return str;
}

std::string getCurDir()
{
	char curDir[_MAX_PATH];
	GetCurrentDirectory(sizeof(curDir), curDir);
	return std::string(curDir);
}