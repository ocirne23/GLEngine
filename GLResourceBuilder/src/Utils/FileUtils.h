#pragma once

#include <string>
#include <vector>

std::string getFileExtension(const std::string& a_filePath);

std::string getFolderPathForFile(const std::string& a_filePath);

std::string getFileTime(const std::string& filePath);

bool listFiles(std::string a_path, std::string a_mask, std::vector<std::string>& a_files);

std::string getExtensionForFilePath(const std::string& a_path);

void createDirectoryForFile(const std::string& a_filePath);

bool fileExists(const std::string& a_filePath);