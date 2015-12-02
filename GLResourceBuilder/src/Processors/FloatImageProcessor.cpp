#include "Processors/FloatImageProcessor.h"

#include "Database/EAssetType.h"
#include "Utils/stb_image.h"
#include "EASTL/string.h"

#include <fstream>
#include <assert.h>

bool FloatImageProcessor::process(const eastl::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	const int type = (int) EAssetType::TEXTURE;
	const bool isFloatImage = true;
	int width, height, numComponents;

	const float* data = stbi_loadf(a_resourcePath.c_str(), &width, &height, &numComponents, 0);
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
	file.write(reinterpret_cast<const char*>(data), width * height * numComponents * sizeof(float));
	file.close();

	delete[] data;
	return true;
}