#pragma once

#include "BuilderCore.h"
#include "Utils/FileUtils.h"

#include <fstream>
#include <vector>
#include <unordered_map>

class IAsset;

enum { MAX_FILEPATH_LENGTH = 255 };

class AssetDatabase
{
public:
	AssetDatabase();
	~AssetDatabase();

public:

	void create(const char* filePath);
	void saveAndClose();
	void openForReading(const char* filePath);
	void close();
	void flush();

	void addAsset(const char* filePath, IAsset* asset);
	IAsset* getAsset(const char* filePath);

public:

	std::vector<IAsset*> getAllAssetsOfType(EAssetType type);
	void packAtlasTextures();

private:

	struct LookupTableEntry
	{
		int64 crc64;
		uint64 byteOffset;
	};

	enum class EOpenMode { CLOSED, WRITEABLE, READONLY };

private:

	void writeAsset(const std::string& filePath, IAsset* asset);
	IAsset* readAsset(const std::string& filePath);

private:

	EOpenMode m_openMode = EOpenMode::CLOSED;
	uint m_assetDataOffset = 0;

	std::fstream m_file;
	uint64 m_assetSizeCounter = 0;

	std::vector<LookupTableEntry> m_fileLookupTable;
	std::unordered_map<std::string, IAsset*> m_unwrittenAssets;
};