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

namespace ASprite {
struct Module {
	std::int8_t w;
	std::int8_t h;
};
struct FrameModule {
	std::int8_t modIndex;
	std::int8_t x;
	std::int8_t y;
	std::int8_t flags; // Transformation Flags (Bit 0: Vertical mirror, Bit 1: Horizontal mirror)
};
struct Frame {
	std::int16_t numModules; // Amount of frame modules
	std::int16_t frameModuleIndex; // Index of the first frame module
};
struct FrameRect {
	std::int8_t x;
	std::int8_t y;
	std::int8_t w;
	std::int8_t h;
};
struct AnimationFrame{
	std::int8_t index; // Frame index
	std::int8_t duration; // Amount of time the frame is shown
	std::int8_t xOffs; // Horizontal offset
	std::int8_t yOffs; // Vertical offset
	std::int8_t flags; // Transformation flags?
};
struct Animation {
	std::int16_t frameCount; // Amount of animation frames
	std::int16_t frameIndex; // Index of the first animation frame
};
}

struct GameloftGraphics {
	int spriteVersion;
	std::vector<ASprite::Module> modules;
	std::vector<ASprite::FrameModule> frameModules;
	std::vector<ASprite::Frame> frames;
	std::vector<ASprite::FrameRect> frameRects;
	std::vector<ASprite::Animation> animations;
	std::vector<ASprite::AnimationFrame> animationFrames;
	std::vector<std::vector<int>> field_14;
	std::vector<std::vector<int>> palettes;
	int paletteCount;
	bool isTransparent;
	std::int16_t bitDepth;
	std::vector<std::vector<unsigned char>> bitmapData;
	std::vector<std::vector<J2MEImage>> sprites;

	// Create graphics from known bitmap and palette data
	void importSprite(std::vector<unsigned char> data, std::vector<int> paletteData, std::int8_t width, std::int8_t height);

	void loadData(std::vector<unsigned char> data, int index = 0);

	/**
	 * Initialize a range of sprites from bitmap data or copy them from one palette to another. Setting copyPalette to -1 will initialize the sprites from bitmap data.
	 * @param palette Palette number
	 * @param start Starting image number
	 * @param stop Last image number, last image if -1
	 * @param copyPalette Palette to copy the sprites from, sprites are constructed from bitmap data if -1.
	 */
	void cacheSpriteImages(int palette, int start, int stop, int copyPalette);
	std::vector<int> method_11(int sprite, int sprPalette);
	J2MEImage getSprite(int palette, int spriteNum);
};

#endif