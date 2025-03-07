#ifndef ASSETPACK_HPP_H
#define ASSETPACK_HPP_H
#include "AssetFile.hpp"
#include <vector>
#include <string>
#include <memory>

// Packed file containing other files
class AssetPack : public GenericAssetFile {
public:
	std::vector<std::unique_ptr<GenericAssetFile>> subFiles;
	AssetPack(std::string fPath, std::string fName, std::vector<unsigned char> fData, int fFormat);
	void refreshData();
};

#endif
