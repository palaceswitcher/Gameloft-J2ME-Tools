#include "GfxAsset.hpp"
#include "AssetFile.hpp"
#include "GameloftGfx.hpp"
#include <vector>
#include <iterator>
#include <string>
//#include <chrono>
//#include <iostream>

GfxAsset::GfxAsset(std::vector<unsigned char> fData, std::string fName, std::string fPath) : GenericAssetFile::GenericAssetFile(fData, fName, fPath) {
	gfx.loadData(data, 0);
	for (int i = 0; i < gfx.paletteCount; i++) {
		gfx.method_1(i, 0, -1, -1);
	}
}

void GfxAsset::updateSpriteImages() {
	for (int i = 0; i < gfx.paletteCount; i++) {
		gfx.method_1(i, 0, -1, -1);
	}
}
