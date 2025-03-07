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

void insertInt(std::vector<unsigned char>& vec, int num) {
	vec.push_back(num);
	vec.push_back((num >> 8) & 0xFF);
	vec.push_back((num >> 16) & 0xFF);
	vec.push_back((num >> 24) & 0xFF);
}

void insertShort(std::vector<unsigned char>& vec, int num) {
	vec.push_back(num);
	vec.push_back((num >> 8) & 0xFF);
}

void AssetPack::refreshData() {
	std::cout << "asset pack data refresh\n";
	std::vector<unsigned char> fData; //Exported pack data (including header)
	std::vector<unsigned char> payload; //Exported pack data
	std::vector<int> fileStartOffs;
	std::vector<int> fileSizes;
	// Compile raw subfile data into payload
	int index = 0;
	for (auto& subFile : subFiles) {
		int fileSize = subFile->data.size();
		fileStartOffs.push_back(index);
		fileSizes.push_back(fileSize);
		payload.insert(payload.end(), subFile->data.begin(), subFile->data.end());
		index += fileSize;
	}
	fileStartOffs.push_back(payload.size()); //Insert last offset at end of file (for offset-only formats)
	int fileCount = subFiles.size();
	switch (format) {
	case FORMAT_PK_OFFS:
		insertShort(fData, fileStartOffs.size()); //Insert file size
		for (auto offs : fileStartOffs) {
			insertInt(fData, offs);
		}
		fData.insert(fData.end(), payload.begin(), payload.end()); //Add payload after header
		break;
	case FORMAT_PK_OFFS_ALT:
		insertInt(fData, fileStartOffs.size());
		for (auto offs : fileStartOffs) {
			insertInt(fData, offs);
		}
		fData.insert(fData.end(), payload.begin(), payload.end()); //Add payload after header
		break;
	case FORMAT_PK_OFFS_SIZE:
		fData.push_back((char)fileCount); //File count
		for (int i = 0; i < fileCount; i++) {
			insertInt(fData, fileStartOffs[i]);
			insertInt(fData, fileSizes[i]);
		}
		fData.insert(fData.end(), payload.begin(), payload.end()); //Add payload after header
		break;
	case FORMAT_PK_MIN:
		for (int i = 0; i < fileCount; i++) {
			insertShort(fData, fileSizes[i]);
			fData.insert(fData.end(), payload.begin()+fileStartOffs[i], payload.begin()+fileStartOffs[i]+fileSizes[i]);
		}
		break;
	}
	data = fData;
}
