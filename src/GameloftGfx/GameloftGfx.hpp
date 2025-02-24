#ifndef GAMELOFTGFX_HPP_
#define GAMELOFTGFX_HPP_
#include "AssetFile.hpp"
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

// Constants
const int ARGB8888 = 0x8888;
const int ARGB4444 = 0x4444;
const int ARGB1555 = 0x5515;
const int RGB565 = 0x6505;
const int BIT_DEPTH_8 = 0x5602;
const int BIT_DEPTH_4 = 0x1600;
const int BIT_DEPTH_2 = 0x0400;
const int BIT_DEPTH_1 = 0x0200;

// Generic replacement for J2ME Image class
class J2MEImage {
public:
	int width;
	int height;
	bool processAlpha;
	std::vector<int> data;
	J2MEImage(std::vector<int> rgb, int pWidth, int pHeight, bool pProcessAlpha);
	J2MEImage();
};

struct GameloftGraphics {
	static int field_23;
	static int field_24;
	static std::vector<unsigned char> field_25;
	static int field_27;// = -1;
	static int field_28;// = -1;
	std::vector<int> argbBitmapData = std::vector<int>(4096);
	std::vector<unsigned char> spriteDims;
	std::vector<unsigned char> field_7;
	std::vector<std::int16_t> field_8;
	std::vector<unsigned char> field_9;
	std::vector<unsigned char> spriteDefs;
	std::vector<unsigned char> field_11;
	std::vector<std::int16_t> field_12;
	std::vector<unsigned char> field_13;
	std::vector<std::vector<int>> field_14;
	std::vector<std::vector<int>> palettes;
	int paletteCount;
	int spritePalette;
	bool isTransparent;
	std::int16_t bitDepth;
	std::vector<unsigned char> bitmapData;
	std::vector<std::int16_t> bitmapInds;
	std::vector<std::vector<J2MEImage>> sprites;
	int field_26 = 0;
	void loadData(std::vector<unsigned char> data, int index = 0);
	void method_1(int var1, int var2, int var3, int var4);
	std::vector<int> method_11(int var1);
	J2MEImage getSprite(int palette, int spriteNum);
};

#endif