#pragma once

#include <vector>
#include <string>

bool listFiles(std::string path, std::string mask, std::vector<std::string>& files, std::vector<std::string>& lastWriteTimesStr);