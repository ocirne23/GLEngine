#pragma once

#include "BuilderCore.h"
#include "Utils/FileUtils.h"

#include <fstream>
#include <vector>

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

	void addAsset(const char* filePath, IAsset* asset);
	IAsset* getAsset(const char* filePath);

private:

	struct LookupTableEntry
	{
		int64 crc64;
		uint64 byteOffset;
	};

	enum class EOpenMode { CLOSED, WRITEABLE, READONLY };

private:

	EOpenMode m_openMode = EOpenMode::CLOSED;
	uint m_assetDataOffset = 0;

	std::fstream m_file;
	uint64 m_assetSizeCounter = 0;

	std::vector<LookupTableEntry> m_lookupTable;
};