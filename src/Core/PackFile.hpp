#include "File.hpp"
#include <vector>
#include <string>
#include <memory>
#ifndef PACKFILE_HPP_ //Include guard
#define PACKFILE_HPP_

// Packed file containing other files
class PackFile : public File {
public:
	std::vector<std::unique_ptr<File>> subFiles;
	int fileCount;
	PackFile(std::string path); //Load packed file from path
};

#endif
