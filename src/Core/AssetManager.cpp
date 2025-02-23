#include "AssetManager.hpp"
#include "AssetFile.hpp"
#include "AssetPack.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <string>
#include <memory>

std::unique_ptr<GenericAssetFile> loadAsset(std::vector<unsigned char> fData, std::string fName, std::string fPath) {
	GenericAssetFile file = GenericAssetFile(fData, fName, fPath);
	switch (file.format) {
	default:
	case FORMAT_FILE_GENERIC:
		return std::make_unique<GenericAssetFile>(file);
		break;
	case FORMAT_FILE_GFX:
		return std::make_unique<GfxAsset>(fData, fName, fPath);
		break;
	case FORMAT_PK_OFFS:
	case FORMAT_PK_OFFS_ALT:
	case FORMAT_PK_OFFS_SIZE:
	case FORMAT_PK_MIN:
		return std::make_unique<AssetPack>(fPath, fName, fData, file.format);
		break;
	}
}