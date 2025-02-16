#include "AssetFile.hpp"
#include <vector>
#include <string>
#include <memory>
#ifndef GFXFILE_HPP_ //Include guard
#define GFXFILE_HPP_

// Packed file containing other files
class GfxFile : public GenericAssetFile {
public:
	std::vector<std::unique_ptr<GenericAssetFile>> subFiles;
	int fileCount;
	GfxFile(std::string path); //Load packed file from path
};

#endif
