#include <vector>
#include <string>
#include <memory>
#ifndef PACKEDFILE_HPP_ //Include guard
#define PACKEDFILE_HPP_

enum FileFormat {
	FORMAT_FILE_GENERIC = 0, //Default or unknown file
	FORMAT_FILE_GFX, //Graphical data file
	FORMAT_FILE_TXT_PK, //String data file
	FORMAT_PK_OFFS, //Pack file (offset-only header)
	FORMAT_PK_OFFS_ALT, //Pack file (alternate offset-only header)
	FORMAT_PK_SIZE, //Pack file (size-only header)
	FORMAT_PK_HYBRID //Pack file (offset and size header)
};

// Generic file
class File {
	std::string path; //Location of the file
public:
	std::string name; //Name of the file
	int format = FORMAT_FILE_GENERIC;
	bool modified = false; //If file was modified since last save
	std::vector<char> data;
	File(); //Default constructor
	File(std::vector<char> data, std::string name);
	File(std::string path); //Load file data from path and identifies its format
};

// Packed file containing other files
class PackFile : public File {
public:
	std::vector<std::unique_ptr<File>> subFiles;
	int fileCount;
	PackFile(std::string path); //Load packed file from path
};

#endif
