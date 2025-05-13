#include "GfxAsset.hpp"
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "GameloftGfx.hpp"
#include "PngDecode.hpp"
#include <vector>
#include <iterator>
#include <string>
#include <iostream>

GfxAsset::GfxAsset(std::vector<unsigned char> fData, std::string fName, std::string fPath) : GenericAssetFile::GenericAssetFile(fData, fName, fPath) {
	gfx.loadData(data, 0);
	for (int i = 0; i < gfx.paletteCount; i++) {
		gfx.cacheSpriteImages(i, 0, -1, -1);
	}
}

void GfxAsset::updateSpriteImages() {
	for (int i = 0; i < gfx.paletteCount; i++) {
		gfx.cacheSpriteImages(i, 0, -1, -1);
	}
}

void GfxAsset::importPng(const char* filename) {
	/*int width;
	int height;
	std::vector<unsigned char> pixels;
	std::vector<int> palette;
	decodePng(filename, width, height, pixels, palette);
	std::cout << "Decoded PNG (" << width << 'x' << height << ")\n";*/
}

void GfxAsset::exportSprite(int sprite, int palette = 0) {
	/*if (palette >= gfx.sprites.size()) {
		std::cerr << "Error exporting sprite: Palette out of range.\n";
		return;
	} else if (sprite >= gfx.sprites[palette].size()) {
		std::cerr << "Error exporting sprite: Sprite out of range.\n";
		return;
	}
	int width;
	int height;
	std::vector<unsigned char> pixels;
	std::vector<int> imgPalette;
	decodePng("test.png", width, height, pixels, imgPalette);
	std::cout << width << "x" << height << " palette size: " << imgPalette.size() << "\n";
		
	gfx.importSprite(pixels, imgPalette, width, height);
	updateSpriteImages();
	std::cout << gfx.bitmapData.back().size() << '\n';
	std::cout << "bit depth: " << gfx.bitDepth << '\n';*/
	/*J2MEImage sprImage = gfx.sprites[palette][sprite];
	int width = sprImage.width;
	int height = sprImage.height;
	SDL_Surface* surf = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_ARGB8888, sprImage.data.data(), width*4);
	SDL_Surface* surf2 = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
	std::string filename = name + ".png";
	stbi_write_png(filename.c_str(), width, height, 4, surf2->pixels, width*4);
	SDL_DestroySurface(surf);
	SDL_DestroySurface(surf2); //Free unused surfaces*/
}

