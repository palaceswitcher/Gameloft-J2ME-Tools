#include "File.hpp"
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
int getIntFromBytes(std::vector<unsigned char>& bytes, int& index) {
	return (bytes[index++]&0xFF) | ((bytes[index++]&0xFF) << 8) | ((bytes[index++]&0xFF) << 16) | ((bytes[index++]&0xFF) << 24);
}

File::File() {
	this->name = "subfile";
	this->format = FORMAT_FILE_GENERIC;
}

File::File(std::vector<unsigned char> data, std::string name) {
	this->name = name;
	this->data = data;
}

/**
 * Load file data from path and identifies its format.
 * @param path File path
 */
File::File(std::string path) {
	std::ifstream inFile(path, std::ifstream::binary|std::ifstream::ate);
	inFile >> std::noskipws;
	data.reserve(inFile.tellg());
	inFile.seekg(0, std::ios::beg);
	std::copy(std::istream_iterator<unsigned char>(inFile), std::istream_iterator<unsigned char>(), std::back_inserter(data)); //Load file data into vector
	this->path = path;
	name = path.substr(path.find_last_of("/\\") + 1);
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
