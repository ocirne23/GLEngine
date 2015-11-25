#include "ByteImageProcessor.h"

#include "Database/Assets/Texture.h"
#include "Database/EAssetType.h"
#include "Database/AssetDatabase.h"

#include "Utils/stb_image.h"
#include <fstream>
#include <assert.h>

bool ByteImageProcessor::process(const std::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	Texture* texture = new Texture(a_resourcePath);
	a_assetDatabase.addAsset(a_resourcePath.c_str(), texture);


	const int type = (int) EAssetType::TEXTURE;
	const bool isFloatImage = false;
	int width, height, numComponents;
	const unsigned char* data = stbi_load(a_resourcePath.c_str(), &width, &height, &numComponents, 0);
	if (!data)
	{
		printf("FAILED TO LOAD IMAGE: %s \n", a_resourcePath.c_str());
		return false;
	}

	std::fstream file(a_resourcePath.c_str(), std::ios::out | std::ios::binary);
	assert(file.is_open());
	file.write(reinterpret_cast<const char*>(&type), sizeof(int));
	file.write(reinterpret_cast<const char*>(&isFloatImage), sizeof(bool));
	file.write(reinterpret_cast<const char*>(&width), sizeof(int));
	file.write(reinterpret_cast<const char*>(&height), sizeof(int));
	file.write(reinterpret_cast<const char*>(&numComponents), sizeof(int));
	file.write(reinterpret_cast<const char*>(data), width * height * numComponents);
	file.close();

	delete[] data;
	return true;
}