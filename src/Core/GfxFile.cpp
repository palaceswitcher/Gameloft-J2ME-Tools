#include "GfxFile.hpp"
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

/**
 * Loads a 32-bit little-endian integer from a vector of chars and moves the index forward.
 * @param bytes Byte data as a vector of chars
 * @param index The index of the variable, passed by reference
 * @returns The 32-bit little-endian integer at the specified index
 */
int getIntFromBytes(std::vector<char>& bytes, int& index) {
	return (bytes[index++]&0xFF) | ((bytes[index++]&0xFF) << 8) | ((bytes[index++]&0xFF) << 16) | ((bytes[index++]&0xFF) << 24);
}

/**
 * Loads a packed file at a specific path
 * @param path File path
 */
GfxFile::GfxFile(std::string path) : File::File(path) {
	std::cout << "packed file constructor start (format: " << FORMAT_PK_OFFS_SIZE << ") (" << path << ")" << std::endl;
	int index = 0;
	if (format == FORMAT_FILE_GFX) {
		;
	}
}
