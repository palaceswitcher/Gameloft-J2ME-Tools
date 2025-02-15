#include "PackFile.hpp"
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
PackFile::PackFile(std::string path) : File::File(path) {
	std::cout << "packed file constructor start (format: " << format << ") (" << path << ")" << std::endl;
	int index = 0;
	if (format == FORMAT_PK_OFFS_SIZE) {
		int fileCount = data[index++];
		int payLoadStart = fileCount*8 + 1; //The index of the first payload byte
		for (int i = 0; i < fileCount; i++) {
			int subFileOffset = getIntFromBytes(data, index);
			int subFileSize = getIntFromBytes(data, index);
			std::cout << "subfile " << i << "\n\tOffset: 0x" << std::hex << subFileOffset << "\n\tSize: 0x" << std::hex << subFileSize << std::endl;

			std::vector<unsigned char> subFileData(data.begin()+payLoadStart+subFileOffset, data.begin()+payLoadStart+subFileOffset+subFileSize);
			std::string subFileName = name.substr(0, name.find_last_of('.')) + '_'+std::to_string(i); //Derive file name from pack file
			subFiles.push_back(std::make_unique<File>(subFileData, subFileName));
		}
	}
}
