#include "Database/Assets/IAsset.h"

#include "Database/Assets/DBAtlasRegion.h"
#include "Database/Assets/DBAtlasTexture.h"
#include "Database/Assets/DBMaterial.h"
#include "Database/Assets/DBMesh.h"
#include "Database/Assets/DBNode.h"
#include "Database/Assets/DBScene.h"
#include "Database/Assets/DBShader.h"
#include "Database/Assets/DBTexture.h"

owner<IAsset*> IAsset::create(EAssetType a_type)
{
	switch (a_type)
	{
	case EAssetType::ATLAS_REGION:
		return new DBAtlasRegion();
	case EAssetType::ATLAS_TEXTURE:
		return new DBAtlasTexture();
	case EAssetType::MATERIAL:
		return new DBMaterial();
	case EAssetType::MESH:
		return new DBMesh();
	case EAssetType::NODE:
		return new DBNode();
	case EAssetType::SCENE:
		return new DBScene();
	case EAssetType::SHADER:
		return new DBShader();
	case EAssetType::TEXTURE:
		return new DBTexture();
	}
	return NULL;
}
