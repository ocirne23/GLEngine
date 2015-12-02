#include "Database/Assets/DBTexture.h"

DBTexture::DBTexture(const std::string& a_sourceFilePath) : m_filePath(a_sourceFilePath)
{

}

uint64 DBTexture::getByteSize() const
{
	return 0;
}

void DBTexture::write(AssetDatabaseEntry& entry)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void DBTexture::read(AssetDatabaseEntry& entry)
{
	throw std::logic_error("The method or operation is not implemented.");
}
