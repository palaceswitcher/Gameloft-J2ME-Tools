#include "GameloftGfx.hpp"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstdint>

//A LOT of this code has been directly translated from decompiled Java, so don't expect it to be completely readable.

J2MEImage::J2MEImage() {
	return;
}

J2MEImage::J2MEImage(std::vector<int> rgb, int pWidth, int pHeight, bool pProcessAlpha) {
	data = rgb;
	width = pWidth;
	height = pHeight;
	processAlpha = pProcessAlpha;
}

/**
 * Loads a 16-bit little-endian integer from a vector of chars in little-endian format and moves the index forward.
 * @param bytes Byte data as a vector of chars
 * @param index The index of the variable, passed by reference
 * @returns The 16-bit little-endian integer at the specified index
 */
std::int16_t bytesToShort(std::vector<unsigned char>& bytes, int& index) {
	return (bytes[index++]&0xFF) + ((bytes[index++]&0xFF) << 8);
}

/**
 * Loads a 32-bit integer from a vector of chars in little-endian format and moves the index forward.
 * @param bytes Byte data as a vector of chars
 * @param index The index of the variable, passed by reference
 * @returns The 32-bit little-endian integer at the specified index
 */
std::int32_t bytesToInt(std::vector<unsigned char>& bytes, int& index) {
	return (bytes[index++]&0xFF) + ((bytes[index++]&0xFF) << 8) + ((bytes[index++]&0xFF) << 16) + ((bytes[index++]&0xFF) << 24);
}

void GameloftGraphics::importSprite(std::vector<unsigned char> data, std::vector<int> paletteData, int width, int height) {
	// Copy to nearest free palette
	if (paletteCount < 16) {
		palettes[paletteCount] = paletteData;
		paletteCount++;
	} else {
		std::cerr << "No available space for palette in graphics.";
		return;
	}
	
	//spriteDims[0] = width;
	//spriteDims[1] = height;
	//bitmapData[0] = std::vector<unsigned char>(data.begin(), data.end());
	spriteDims.push_back(width);
	spriteDims.push_back(height);
	bitmapData.push_back(std::vector<unsigned char>(data.begin(), data.end()));
}

void GameloftGraphics::loadData(std::vector<unsigned char> data, int index) {
	index = 6; //Starts after signature
	int spriteDimCount = bytesToShort(data, index); //How many sprite sizes
	if (spriteDimCount != 0) {
		std::copy(data.begin()+index, data.begin()+index+spriteDimCount*2, back_inserter(spriteDims));
		index += spriteDimCount*2;
	}

	int spriteDefCount = bytesToShort(data, index);
	if (spriteDefCount != 0) {
		std::vector<int> field_10;
		std::copy(data.begin()+index, data.begin()+index+spriteDefCount*4, back_inserter(field_10));
		index += spriteDefCount*4;
	}

	int var7 = bytesToShort(data, index);
	if (var7 != 0) {
		field_7.reserve(var7);
		field_8.reserve(var7);
		for (int i = 0; i < var7; i++) {
			field_7.push_back(data[index++]);
			index++;
			field_8.push_back(bytesToShort(data, index));
		}

		int var53 = var7 * 4;
		field_9.reserve(var53);

		for (int i = 0; i < var53; i++) {
			field_9.push_back(data[index++]);
		}
	}

	int var54 = bytesToShort(data, index);
	if (var54 != 0) {
		int field_13Size = var54*5;
		std::copy(data.begin()+index, data.begin()+index+field_13Size, back_inserter(field_13));
		index += field_13Size;
	}

	int var55 = bytesToShort(data, index);
	if (var55 != 0) {
		field_11.reserve(var55);
		field_12.reserve(var55);
		for (int i = 0; i < var55; i++) {
			field_11.push_back(data[index++]);
			index++;
			field_12.push_back(getShortFromBytes(data, index));
		}
	}

	if (spriteDimCount > 0) {
		int paletteFormat = bytesToShort(data, index);
		paletteCount = data[index++] & 0xFF;
		int paletteSize = data[index++] & 0xFF;
		// Reserve empty space for palettes
		for (int i = 0; i < 16; i++) {
			palettes.push_back(std::vector<int>());
		}

		for (int i = 0; i < paletteCount; i++) {
			switch (paletteFormat) {
			case ARGB8888:
				for (int c = 0; c < paletteSize; c++) {
					int palColor = bytesToInt(data, index);
					if ((palColor & 0xFF000000) != 0xFF000000) {
						isTransparent = true;
					}

					palettes[i].push_back(palColor);
				}
				break;
			case ARGB4444:
				for (int c = 0; c < paletteSize; c++) {
					int palColor = bytesToShort(data, index);
					if ((palColor & 0xF000) != 61440) {
						isTransparent = true;
					}

					palettes[i].push_back((palColor & 0xF000) << 16 | (palColor & 0xF000) << 12 | (palColor & 0xF00) << 12 | (palColor & 0xF00) << 8 | (palColor & 0xF0) << 8 | (palColor & 0xF0) << 4 | (palColor & 0xF) << 4 | palColor & 0xF);
				}
				break;
			case ARGB1555:
				for (int c = 0; c < paletteSize; c++) {
					int palColor = bytesToShort(data, index);
					int alpha = 0xFF000000;
					if ((palColor & 0x8000) != 0x8000) {
						alpha = 0;
						isTransparent = true;
					}

					palettes[i].push_back(alpha | (palColor & 0x7C00) << 9 | (palColor & 0x3E0) << 6 | (palColor & 0x1F) << 3);
				}
				break;
			case RGB565:
				for (int c = 0; c < paletteSize; c++) { 
					int palColor = bytesToShort(data, index);
					int alpha = 0xFF000000;
					if (palColor == 0xF81F) {
						alpha = 0;
						isTransparent = true;
					}

					palettes[i].push_back(alpha | (palColor & 0xF800) << 8 | (palColor & 0x7E0) << 5 | (palColor & 0x1F) << 3);
				}
				break;
			}
		}

		bitDepth = bytesToShort(data, index);
		if (spriteDimCount > 0) {
			for (int i = 0; i < spriteDimCount; i++) {
				int bitmapSize = bytesToShort(data, index);
				std::vector<unsigned char> vec(data.begin()+index, data.begin()+index+bitmapSize);
				bitmapData.push_back(vec);
				index += bitmapSize;
			}
		}
	}
}

void GameloftGraphics::method_1(int palette, int start, int stop, int copyPalette) {
	if (!spriteDims.empty()) {
		if (stop == -1) {
			stop = (spriteDims.size() / 2) - 1; //Get all sprites up to the last
		}

		//sprites.clear();
		if (sprites.empty()) {
			for (int i = 0; i < paletteCount; i++) {
				sprites.push_back({});
			}
		}
		if (sprites.size() <= palette) {
			for (int i = sprites.size()-1; i < palette; i++) {
				sprites.push_back({});
			}
		}

		sprites[palette].clear();
		/*if (sprites[palette].empty()) {
			for (int i = 0; i < (spriteDims.size() / 2); i++) {
				sprites[palette].push_back(J2MEImage());
			}
		}*/
		/*if (sprites[palette].size() <= stop+1) {
			for (int i = 0; i <= stop; i++) {
				sprites[palette].push_back(J2MEImage());
			}
		}*/

		if (copyPalette >= 0) {
			// Copy from one palette to another
			for (int i = start; i <= stop; i++) {
				sprites[palette][i] = sprites[copyPalette][i];
			}
		} else {
			for (int i = start; i <= stop; i++) {
				int var7 = i * 2;
				int spriteWidth = spriteDims[var7] & 0xFF; //Get width
				int spriteHeight = spriteDims[var7 + 1] & 0xFF; //Get height
				std::vector<int> argbData;
				if (spriteWidth > 0 && spriteHeight > 0 && !((argbData = method_11(i, palette)).empty())) {
					bool sprIsTransparent = false;
					int spritePixelCount = spriteWidth * spriteHeight;

					for (int j = 0; j < spritePixelCount; j++) {
						if ((argbData[j] & 0xFF000000) != 0xFF000000) {
							sprIsTransparent = true;
							break;
						}
					}

					sprites[palette].push_back(J2MEImage(argbData, spriteWidth, spriteHeight, sprIsTransparent));
				}
			}
		}
	}
}

std::vector<int> GameloftGraphics::method_11(int sprite, int sprPalette) {
	if (!bitmapData[sprite].empty()) {
		int var2 = sprite * 2;
		int spriteWidth = spriteDims[var2] & 0xFF;
		int spriteHeight = spriteDims[var2 + 1] & 0xFF;
		std::vector<int> palette;
		if (sprPalette >= palettes.size() || (palette = palettes[sprPalette]).empty()) {
			return {};
		} else {
			int index = 0;
			int argbBitmapInd = 0;
			int spritePixelCount = spriteWidth * spriteHeight; //Total amount of pixels
			std::vector<int> argbBitmapData(spritePixelCount);
			if (bitDepth != 0x27F1) {
				if (bitDepth == BIT_DEPTH_4) {
					while (argbBitmapInd < spritePixelCount) {
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 4 & 0xF];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] & 0xF];
						index++;
					}
				} else if (bitDepth == BIT_DEPTH_2) {
					while (argbBitmapInd < spritePixelCount) {
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 6 & 3];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 4 & 3];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 2 & 3];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] & 3];
						index++;
					}
				} else if (bitDepth == BIT_DEPTH_1) {
					while (argbBitmapInd < spritePixelCount) {
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 7 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 6 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 5 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 4 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 3 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 2 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] >> 1 & 1];
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index] & 1];
						index++;
					}
				} else if (bitDepth == BIT_DEPTH_8) {
					while (argbBitmapInd < spritePixelCount) {
						argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index++] & 0xFF];
					}
				} else if (bitDepth == 0x56F2) {
					while (argbBitmapInd < spritePixelCount) {
						int var26 = bitmapData[sprite][index++] & 0xFF;
						// If not RLE tag
						if (var26 > 127) {
							for (int i = var26 - 0x80;
							i-- > 0;
							argbBitmapData[argbBitmapInd++] = palette[bitmapData[sprite][index++] & 0xFF]) {}
						// If RLE tag
						} else {
							for (int i = palette[bitmapData[sprite][index++] & 0xFF];
							var26-- > 0;
							argbBitmapData[argbBitmapInd++] = i) {}
						}
					}
				}
			} else {
				while (argbBitmapInd < spritePixelCount) {
					int var11 = bitmapData[sprite][index++] & 0xFF;
					// If RLE tag
					if (var11 > 127) {
						int var12 = bitmapData[sprite][index++] & 0xFF;
						int var13 = palette[var12];

						for(int var25 = var11 - 0x80;
							var25-- > 0;
							argbBitmapData[argbBitmapInd++] = var13) {}
					// If uncompressed data
					} else {
						argbBitmapData[argbBitmapInd++] = palette[var11];
					}
				}
			}

			return argbBitmapData;
		}
	} else {
		return {};
	}
}

/**
 * Returns sprite as a J2ME image. Returns an empty image on failure.
 * @param palette Palette number of sprite
 * @param spriteNum Number of sprite
 */
J2MEImage GameloftGraphics::getSprite(int palette, int spriteNum) {
	int spritePalette = 0;
	int spriteBoxWidth = spriteDims[spriteNum*2] & 0xFF;
	int spriteBoxHeight = spriteDims[spriteNum*2 + 1] & 0xFF;
	J2MEImage image;
	if (spritePalette < paletteCount && spriteNum < sprites[spritePalette].size() &&
		/*!sprites.empty() &&*/ !sprites[spritePalette].empty()) {
		image = sprites[spritePalette][spriteNum];
	} else {
		std::vector<int> rawImgData = method_11(spriteNum, palette);
		if (rawImgData.empty()) {
			spriteBoxWidth = 0;
			spriteBoxHeight = 0;
		}
		image = {rawImgData, spriteBoxWidth, spriteBoxHeight, isTransparent}; //Empty image
	}
	return image;
}
