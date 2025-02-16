#include "AssetFile.hpp"
#include <vector>
#include <string>
#include <memory>
#ifndef PACKFILE_HPP_ //Include guard
#define PACKFILE_HPP_

// Packed file containing other files
class PackFile : public GenericAssetFile {
public:
	std::vector<std::unique_ptr<GenericAssetFile>> subFiles;
	int fileCount;
	PackFile(std::vector<unsigned char> data, std::string name, std::string path = ""); //Load packed file from path
};

#endif
