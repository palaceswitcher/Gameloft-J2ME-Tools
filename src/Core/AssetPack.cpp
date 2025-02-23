#include "AssetPack.hpp"
#include "AssetFile.hpp"
#include "AssetManager.hpp"
#include <vector>
#include <string>
#include <memory>
#include <iostream>

AssetPack::AssetPack(std::string fPath, std::string fName, std::vector<unsigned char> fData, int fFormat) {
	path = fPath;
	name = fName;
	data = fData;
	format = fFormat; //Copy attributes of generic constructor file
	std::cout << "packed file constructor start (format: " << format << ") (" << path << ")" << std::endl;
	int index = 0;
	std::vector<int> subFileOffsets;
	std::vector<int> subFileSizes;
	int assetCount = 0; //Amount of assets in asset pack
	int payLoadStart = 0; //Start index of the payload in the asset pack
	if (format == FORMAT_PK_OFFS) {
		assetCount = getShortFromBytes(data, index);
		payLoadStart = assetCount*4 + 2;
		for (int i = 0; i < assetCount; i++) {
			subFileOffsets.push_back(getIntFromBytes(data, index));
		}
		assetCount--;
		for (int i = 0; i < assetCount; i++) {
			subFileSizes.push_back(subFileOffsets[i+1] - subFileOffsets[i]);
		}
	} else if (format == FORMAT_PK_OFFS_ALT) {
		assetCount = getIntFromBytes(data, index);
		payLoadStart = assetCount*4 + 5;
		subFileOffsets.push_back(0);
		for (int i = 0; i < assetCount; i++) {
			subFileOffsets.push_back(getIntFromBytes(data, index));
		}
		for (int i = 0; i < assetCount; i++) {
			subFileSizes.push_back(subFileOffsets[i+1] - subFileOffsets[i]);
		}
	} else if (format == FORMAT_PK_OFFS_SIZE) {
		assetCount = data[index++];
		payLoadStart = assetCount*8 + 1; //The index of the first payload byte
		for (int i = 0; i < assetCount; i++) {
			subFileOffsets.push_back(getIntFromBytes(data, index));
			subFileSizes.push_back(getIntFromBytes(data, index));
		}
	} else if (format == FORMAT_PK_MIN) {
		std::cout << "pack min constructor called\n";
		int index = 0;
		while (index < data.size()) {
			int fileSize = getShortFromBytes(data, index);
			subFileOffsets.push_back(index);
			subFileSizes.push_back(fileSize);
			index += fileSize;
			assetCount++;
		}
	}
	// Split sub-assets
	for (int i = 0; i < assetCount; i++) {
		std::vector<unsigned char> subFileData(data.begin()+payLoadStart+subFileOffsets[i], data.begin()+payLoadStart+subFileOffsets[i]+subFileSizes[i]);
		std::string subFileName = name.substr(0, name.find_last_of('.')) + '_'+std::to_string(i); //Derive file name from asset pack filename
		subFiles.push_back(loadAsset(subFileData, subFileName, ""));
	}
}