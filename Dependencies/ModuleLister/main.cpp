#include <fstream>
#include <filesystem>

std::string getCurDir()
{
	return std::filesystem::current_path().string();
}

void writeToLog(std::ofstream& file, std::string str)
{
	file.write(str.c_str(), str.length());
	file.put(' ');
}

int main(int argc, char* argv[])
{
	const std::string directory = getCurDir();
	const std::string fileName = directory + "/modulelist";
	std::ofstream logFile(fileName, std::ios_base::out);
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.path().extension().string() == ".ifc")
		{
			writeToLog(logFile, entry.path().filename().string());
		}
	}
	logFile.close();
	return 0;
}