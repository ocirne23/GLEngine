#include "VSCmdProcess.h"
#include "FileUtils.h"

#include <fstream>

int main()
{
	namespace fs = std::filesystem;
	auto root = fs::current_path().parent_path().parent_path();
	
	auto preprocessOutput = root;
	preprocessOutput /= "i";
	std::filesystem::create_directory(preprocessOutput);

	printf("root:%s\n", root.filename().string().c_str());

	VSCmdProcess cmdProcess;
	cmdProcess.changeDirectory(root.string());

	listFilesWithExt(root, ".ixx", [&cmdProcess](const std::filesystem::directory_entry & file)
		{
			const std::string ixxDir = file.path().parent_path().string();
			const std::string ixxNameNoExt = file.path().stem().string();

			//cmdProcess.writeToConsole(preprocessCmd);
		}
	);


	cmdProcess.writeToConsole("hai\n");
	Sleep(1000);
	cmdProcess.writeToConsole("ello\n");
	Sleep(1000);
	cmdProcess.writeToConsole("cl\n");

	DWORD numWritten = 0;
	while (1)
	{
		Sleep(100);
		std::string str = readUserInput();
		if (str == "exit\n")
		{
			break;
		}
		cmdProcess.writeToConsole(str);
	}
	return 0;
}



/*
int main()
{
	printf("Hai\n");
	/*


	
	int i = 0;
	do
	{
		i = std::cin.get();
	} while (i != '0');
	

	return 0;
}*/