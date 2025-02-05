#include "PackedFile.hpp"
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

File::File() {
	this->name = "subfile";
	this->format = FORMAT_FILE_GENERIC;
}

File::File(std::vector<char> data, std::string name) {
	this->name = name;
	this->data = data;
}

/**
 * Load file data from path and identifies its format.
 * @param path File path
 */
File::File(std::string path) {
	std::ifstream inFile(path, std::ios::binary);
	data = std::vector<char>(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>()); //Load file data into vector
	this->path = path;
	name = path.substr(path.find_last_of("/\\") + 1);
	int packFileSize = data.size(); //Get size of file
	format = FORMAT_FILE_GENERIC; //Assume file is generic in case the below checks fail

	// Attempt to match for hybrid format
	std::vector<int> fileOffsets;
	bool matchFound = true; //This is true until proven otherwise
	int index = 0;
	int fileCount = data[index++];
	if (fileCount * 8 + 1 < packFileSize && fileCount > 0) {
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);
			int subFileSize = getIntFromBytes(data, index);

			if (subFileOffset >= packFileSize && subFileSize >= packFileSize) {
				index = 0; //Clear any data found if the header is found to be invalid
				matchFound = false; //We can safely assume that it failed the match if any metadata was found to be invalid
				break;
			}
		}
		if (matchFound) {
			format = FORMAT_PK_HYBRID;
			return;
		} else {
			matchFound = true; //Assume true for the next check until proven otherwise
		}
	}
	// Match for offset format
	fileCount = (data[index++]&0xFF) | ((data[index++]&0xFF) << 8);
	fileOffsets.reserve(fileCount);
	if (!matchFound && fileCount * 4 + 2 < packFileSize && fileCount > 0) {
		// Get offsets first so we can figure out sizes after
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);

			if (subFileOffset >= packFileSize - (fileCount*4 + 2)) {
				index = 0;
				matchFound = false;
				break;
			} else {
				fileOffsets[i] = subFileOffset;
			}
		}
		// Validate sizes
		for (int i = 0; i < fileCount - 1; i++) {
			int subFileSize = fileOffsets[i+1] - fileOffsets[i];

			if (subFileSize >= packFileSize || subFileSize < 0) {
				index = 0;
				matchFound = false;
				break;
			}
		}
		if (matchFound) {
			std::cout << "match\n";
			format = FORMAT_PK_OFFS;
			return;
		}
	}
	std::cout << "format detect end\n";
}

/**
 * Loads a packed file at a specific path
 * @param path File path
 */
PackFile::PackFile(std::string path) : File::File(path) {
	std::cout << "packed file constructor start (format: " << FORMAT_PK_HYBRID << ") (" << path << ")\n";
	int index = 0;
	if (format == FORMAT_PK_HYBRID) {
		int fileCount = data[index++];
		int payLoadStart = fileCount*8 + 1; //The index of the first payload byte
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);
			int subFileSize = getIntFromBytes(data, index);
			std::cout << "subfile " << i << "\n\tOffset: 0x" << std::hex << subFileOffset << "\n\tSize: 0x" << std::hex << subFileSize << std::endl;

			std::vector<char> subFileData(data.begin()+payLoadStart+subFileOffset, data.begin()+payLoadStart+subFileOffset+subFileSize);
			std::string subFileName = name.substr(0, name.find_last_of('.')) + '_'+std::to_string(i); //Derive file name from pack file
			subFiles.push_back(std::make_unique<File>(subFileData, subFileName));
		}
	}
}
