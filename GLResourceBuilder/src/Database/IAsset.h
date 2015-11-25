#pragma once

#include "BuilderCore.h"
#include "Database/EAssetType.h"

#include <string>
#include <ostream>
#include <istream>

class IAsset
{
public:

	IAsset(const std::string& a_databaseEntryName) : m_databaseEntryName(a_databaseEntryName) {}
	virtual ~IAsset() {}

	virtual uint getByteSize() const        = 0;
	virtual EAssetType getAssetType() const = 0;
	virtual void write(std::ostream& file)  = 0;
	virtual void read(std::istream& file)   = 0;

	const std::string& getDatabaseEntryName() const { return m_databaseEntryName; }

private:

	std::string m_databaseEntryName;
};