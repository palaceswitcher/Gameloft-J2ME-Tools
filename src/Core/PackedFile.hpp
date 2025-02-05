#include <vector>
#include <string>
#include <memory>
#ifndef PACKEDFILE_HPP_ //Include guard
#define PACKEDFILE_HPP_

enum FileFormat {
	FORMAT_FILE_GENERIC, //Default/unknown file
	FORMAT_FILE_GFX,
	FORMAT_FILE_TXT_PK,
	FORMAT_PK_OFFS,
	FORMAT_PK_OFFS_ALT,
	FORMAT_PK_SIZE,
	FORMAT_PK_HYBRID
};

// Generic file
class File {
	std::string path; //Location of the file
public:
	std::string name; //Name of the file
	int format = FORMAT_FILE_GENERIC;
	std::vector<char> data;
	File(); //Default constructor
	File(std::vector<char> data, std::string name);
	File(std::string path); //Load file data from path and identifies its format
};

// Packed file containing other files
class PackFile : public File {
public:
	std::vector<std::unique_ptr<File>> subFiles;
	bool collapsed = false;
	bool modified = false;
	int fileCount;
	PackFile(std::string path); //Load packed file from path
};

#endif
