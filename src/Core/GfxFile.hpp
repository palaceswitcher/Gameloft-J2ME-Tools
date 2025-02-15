#include "File.hpp"
#include <vector>
#include <string>
#include <memory>
#ifndef GFXFILE_HPP_ //Include guard
#define GFXFILE_HPP_

// Packed file containing other files
class GfxFile : public File {
public:
	std::vector<std::unique_ptr<File>> subFiles;
	int fileCount;
	GfxFile(std::string path); //Load packed file from path
};

#endif
