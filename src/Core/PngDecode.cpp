#include "PngDecode.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <libimagequant.h>
#include <vector>
#include <iostream>
#include <cstdio>

#include <fstream>

bool decodePng(const char* filename, int& width, int& height, std::vector<unsigned char>& pixelIndices, std::vector<int>& palette) {
	// Load PNG and quantize it
	int channels;
	unsigned char* pixels = stbi_load(filename, &width, &height, &channels, 4);
	liq_attr* attr = liq_attr_create();
	liq_set_max_colors(attr, 16);
	liq_image* image = liq_image_create_rgba(attr, pixels, width, height, 0);
	liq_result* result;
	if (liq_image_quantize(image, attr, &result) != LIQ_OK) {
		std::cerr << "Error: Image quantization failed.\n";
		return false;
	}
	liq_set_dithering_level(result, 1.0f);

	// Write pixel indices
	std::vector<unsigned char> pngPixelIndices(width*height);
	std::ofstream outfile("a", std::ios::binary);
	outfile.write((char*)pngPixelIndices.data(), pngPixelIndices.size());
	outfile.close();
	liq_write_remapped_image(result, image, pngPixelIndices.data(), pngPixelIndices.size());
	for (int i = 0; i < pngPixelIndices.size(); i += 2) {
		pixelIndices.push_back((pngPixelIndices[i] << 4) | (pngPixelIndices[i+1] & 0xF));
	}

	// Get quantized palette
	const liq_palette* pngPalette = liq_get_palette(result);
	palette.clear();
	for (int i = 0; i < pngPalette->count; i++) {
		liq_color c = pngPalette->entries[i];
		int argb = (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
		palette.push_back(argb);
	}
	
	// Free unused data
	liq_result_destroy(result);
	liq_image_destroy(image);
	liq_attr_destroy(attr);
	stbi_image_free(pixels);

	return true;
}
