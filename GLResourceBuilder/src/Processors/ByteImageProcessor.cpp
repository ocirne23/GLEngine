#include "ByteImageProcessor.h"

#include "../Utils/stb_image.h"

#include <fstream>
#include <assert.h>

bool ByteImageProcessor::process(const char* inResourcePath, const char* outResourcePath)
{
	int type = ResourceType_BYTEIMAGE;
	int width, height, numComponents;

	const unsigned char* data = stbi_load(inResourcePath, &width, &height, &numComponents, 0);
	if (!data)
	{
		printf("FAILED TO LOAD IMAGE: %s \n", inResourcePath);
		return false;
	}
	std::fstream file(outResourcePath, std::ios::out | std::ios::binary);

	assert(file.is_open());
	file.write(reinterpret_cast<const char*>(&type), sizeof(int));
	file.write(reinterpret_cast<const char*>(&width), sizeof(int));
	file.write(reinterpret_cast<const char*>(&height), sizeof(int));
	file.write(reinterpret_cast<const char*>(&numComponents), sizeof(int));
	file.write(reinterpret_cast<const char*>(data), width * height * numComponents);
	file.close();

	delete[] data;
	return true;
}
