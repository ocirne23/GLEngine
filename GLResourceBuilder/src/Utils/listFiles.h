#pragma once

#include <vector>
#include <string>

bool listFiles(std::string a_path, std::string a_mask, std::vector<std::string>& a_files, std::vector<std::string>& a_lastWriteTimesStr);