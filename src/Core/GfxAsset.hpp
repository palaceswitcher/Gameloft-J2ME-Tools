#ifndef GFXASSET_HPP_
#define GFXASSET_HPP_
#include "AssetFile.hpp"
#include "GameloftGfx.hpp"
#include <vector>
#include <string>
#include <memory>

// Graphical data file
class GfxAsset : public GenericAssetFile {
public:
	GameloftGraphics gfx;
	GfxAsset(std::vector<unsigned char> fData, std::string fName, std::string fPath = ""); //Load GFX file from path
	void updateSpriteImages();

	// Import sprite from PNG
	void importPng(const char* filename);

	// Export sprite
	void exportSprite(int number, int palette);
};

#endif