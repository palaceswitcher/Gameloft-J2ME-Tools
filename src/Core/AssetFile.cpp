#include "AssetFile.hpp"
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

/**
 * Loads a 16-bit little-endian integer from a vector of chars in little-endian format and moves the index forward.
 * @param bytes Byte data as a vector of chars
 * @param index The index of the variable, passed by reference
 * @returns The 16-bit little-endian integer at the specified index
 */
std::int16_t getShortFromBytes(std::vector<unsigned char>& bytes, int& index) {
	return (bytes[index++]&0xFF) + ((bytes[index++]&0xFF) << 8);
}

/**
 * Loads a 32-bit integer from a vector of chars in little-endian format and moves the index forward.
 * @param bytes Byte data as a vector of chars
 * @param index The index of the variable, passed by reference
 * @returns The 32-bit little-endian integer at the specified index
 */
std::int32_t getIntFromBytes(std::vector<unsigned char>& bytes, int& index) {
	return (bytes[index++]&0xFF) + ((bytes[index++]&0xFF) << 8) + ((bytes[index++]&0xFF) << 16) + ((bytes[index++]&0xFF) << 24);
}

GenericAssetFile::GenericAssetFile() {
	this->name = "subfile";
	this->format = FORMAT_FILE_GENERIC;
}

/**
 * Creates a file from data and attempts to identify its file format.
 * @param fData File data
 * @param fName File name
 * @param fPath File path
 */
GenericAssetFile::GenericAssetFile(std::vector<unsigned char> fData, std::string fName, std::string fPath) {
	data = fData;
	name = fName;
	path = fPath;
	int packFileSize = data.size(); //Get size of file
	format = FORMAT_FILE_GENERIC; //Assume file is generic in case the below checks fail

	// Attempt to match for offset+size format
	std::vector<int> fileOffsets;
	bool matchFound = true; //This is true until proven otherwise
	int index = 0;
	int fileCount = data[index++];
	if (fileCount * 8 + 1 < packFileSize && fileCount > 0) {
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);
			int subFileSize = getIntFromBytes(data, index);

			// If the expected offset locations exceed the bounds of the file, it can be assumed that this format does not match
			if (subFileOffset >= packFileSize - (fileCount * 8 + 1) || subFileSize >= packFileSize - (fileCount * 8 + 1)) {
				index = 0; //Clear any data found if the header is found to be invalid
				matchFound = false; //We can safely assume that it failed the match if any metadata was found to be invalid
				break;
			}
		}
		if (matchFound) {
			format = FORMAT_PK_OFFS_SIZE;
			return;
		}
	}
	// Match for offset format
	matchFound = true; //Assume true for the next check until proven otherwise
	fileCount = (data[index++]&0xFF) | ((data[index++]&0xFF) << 8);
	fileOffsets.reserve(fileCount);
	if (fileCount * 4 + 2 < packFileSize && fileCount > 0) {
		// Get offsets first so we can figure out sizes after
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);

			if (subFileOffset > packFileSize - (fileCount*4 + 2)) {
				index = 0;
				matchFound = false;
				break;
			} else {
				fileOffsets[i] = subFileOffset;
			}
		}
		// Validate sizes
		for (int i = 0; matchFound && i < fileCount - 1; i++) {
			int subFileSize = fileOffsets[i+1] - fileOffsets[i];

			if (subFileSize > packFileSize - (fileCount*4 + 2) || subFileSize < 0) {
				index = 0;
				matchFound = false;
				break;
			}
		}
		if (matchFound) {
			format = FORMAT_PK_OFFS;
			return;
		}
	}

	// Match for alt offset format
	matchFound = true;
	fileCount = getIntFromBytes(data, index);
	if (fileCount * 4 + 5 < packFileSize && fileCount > 0) {
		std::vector<int> fileOffsets(fileCount);
		// Get sizes first so we can figure out offsets after
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);

			if (subFileOffset > packFileSize - (fileCount*4 + 4)) {
				index = 0;
				matchFound = false;
				break;
			} else {
				fileOffsets[i] = subFileOffset;
			}
		}
		// Validate offsets
		for (int i = 0; i < fileCount - 1; i++) {
			int subFileSize = (fileOffsets[i+1] - fileOffsets[i]) + 1;

			if (subFileSize > packFileSize - (fileCount*4 + 4) || subFileSize < 0) {
				index = 0;
				matchFound = false;
				break;
			}
		}
		if (data[index] == 0x00 && matchFound) {
			format = FORMAT_PK_OFFS_ALT;
			return;
		}
	}

	//Match for graphics format
	matchFound = true;
	const unsigned char gfxFileSig[] = {0xDF, 0x03, 0x01, 0x01, 0x01, 0x01};
	for (int i = 0; i < 6; i++) {
		if (data[i] != gfxFileSig[i]) {
			matchFound = false;
			break;
		}
	}
	if (matchFound) {
		format = FORMAT_FILE_GFX;
	}
}
