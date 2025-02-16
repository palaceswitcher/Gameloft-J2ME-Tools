#include "PackFile.hpp"
#include <cstdint>
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

/**
 * Loads a packed file at a specific path
 * @param path File path
 */
PackFile::PackFile(std::vector<unsigned char> data, std::string name, std::string path) : GenericAssetFile::GenericAssetFile(data, name, path) {
	std::cout << "packed file constructor start (format: " << format << ") (" << path << ")" << std::endl;
	int index = 0;
	if (format == FORMAT_PK_OFFS) {
		std::int16_t fileCount = getShortFromBytes(data, index);
		int payLoadStart = fileCount*4 + 2;
		std::vector<int> subFileOffsets(fileCount);
		for (int i = 0; i < fileCount; i++) {
			subFileOffsets[i] = getIntFromBytes(data, index);
		}
		fileCount--;
		for (int i = 0; i < fileCount; i++) {
			int subFileSize = subFileOffsets[i+1] - subFileOffsets[i];
			std::vector<unsigned char> subFileData(data.begin()+payLoadStart+subFileOffsets[i], data.begin()+payLoadStart+subFileOffsets[i]+subFileSize);
			std::string subFileName = name.substr(0, name.find_last_of('.')) + '_'+std::to_string(i); //Derive file name from pack file
			subFiles.push_back(std::make_unique<GenericAssetFile>(subFileData, subFileName));
		}
	} else if (format == FORMAT_PK_OFFS_SIZE) {
		int fileCount = data[index++];
		int payLoadStart = fileCount*8 + 1; //The index of the first payload byte
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);
			int subFileSize = getIntFromBytes(data, index);
			std::cout << "subfile " << i << "\n\tOffset: 0x" << std::hex << subFileOffset << "\n\tSize: 0x" << std::hex << subFileSize << std::endl;

			std::vector<unsigned char> subFileData(data.begin()+payLoadStart+subFileOffset, data.begin()+payLoadStart+subFileOffset+subFileSize);
			std::string subFileName = name.substr(0, name.find_last_of('.')) + '_'+std::to_string(i); //Derive file name from pack file
			subFiles.push_back(std::make_unique<GenericAssetFile>(subFileData, subFileName));
		}
	}
}
