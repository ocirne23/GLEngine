#include "GW2SongProcessor.h"

#include <fstream>
#include <assert.h>
#include <vector>

bool GW2SongProcessor::process(const char* inResourcePath, const char* outResourcePath)
{
	std::fstream inFile(inResourcePath, std::ios::in | std::ios::binary);
	assert(inFile.is_open());
	std::fstream outFile(outResourcePath, std::ios::out | std::ios::binary);
	assert(outFile.is_open());

	inFile.seekg(0, std::ios::end);
	int filesize = (int) inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	std::vector<char> bytes(filesize);
	inFile.read(&bytes[0], filesize);

	int octave = 1;
	bool previousCharIsKey = false;
	char previous = 0;
	for (int i = 0; i < bytes.size(); ++i)
	{
		char c = bytes[i];
		char next = i < bytes.size() - 1 ? bytes[i + 1] : c;

		switch (c)
		{
		case '1':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('1');
			previousCharIsKey = true;
			previous = c;
			break;
		case '2':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('2');
			previousCharIsKey = true;
			previous = c;
			break;
		case '3':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('3');
			previousCharIsKey = true;
			previous = c;
			break;
		case '4':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('4');
			previousCharIsKey = true;
			previous = c;
			break;
		case '5':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('5');
			previousCharIsKey = true;
			previous = c;
			break;
		case '6':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('6');
			previousCharIsKey = true;
			previous = c;
			break;
		case '7':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('7');
			previousCharIsKey = true;
			previous = c;
			break;
		case '8':
			if (previousCharIsKey)
				outFile.put('/');
			outFile.put('8');
			previousCharIsKey = true;
			previous = c;
			break;
		case ' ':
			if (octave == 0)
			{
				if (previous != 'd' && next != 'u')
					outFile.put(' ');
			}
			else if (octave == 1)
			{
				if (previous != 'u')
					outFile.put(' ');
			}
			else if (octave == 2)
			{
				if (!(next == 'd' || previous == 'u'))
					outFile.put(' ');
			}

			previousCharIsKey = false;
			previous = c;
			break;
		case 'u':
			if (octave < 2)
				octave++;
			if (octave == 1)
				outFile.put(']');
			else if (octave == 2)
				outFile.put('{');
			previousCharIsKey = (previous != ' ');
			previous = c;
			break;
		case 'd':
			if (octave > 0)
				octave--;
			
			if (octave == 0)
				outFile.put('[');
			else if (octave == 1)
				outFile.put('}');
			previousCharIsKey = (previous != ' ');
			previous = c;
			break;
		case '{':
			previousCharIsKey = false;
			previous = c;
			break;
		case '}':
			previousCharIsKey = false;
			previous = c;
			break;
		case '\n':
			break;
		case '\r':
			break;
		default:
			//printf("invalid character : %c \n", c);
			break;
		}
	}

	inFile.close();
	outFile.close();

	return true;
}
