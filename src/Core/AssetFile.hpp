#ifndef ASSETFILE_HPP_ //Include guard
#define ASSETFILE_HPP_
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

std::int16_t getShortFromBytes(std::vector<unsigned char>& bytes, int& index);
std::int32_t getIntFromBytes(std::vector<unsigned char>& bytes, int& index);

enum FileFormat {
	FORMAT_FILE_GENERIC = 0, //Default or unknown file
	FORMAT_FILE_GFX, //Graphical data file
	FORMAT_FILE_TXT_PK, //String data file
	FORMAT_PK_OFFS, //Asset pack (offset-only header)
	FORMAT_PK_OFFS_ALT, //Asset pack (alternate offset-only header)
	FORMAT_PK_OFFS_SIZE, //Asset pack (offset and size header)
	FORMAT_PK_MIN //Minimalistic asset pack
};

// Generic file
class GenericAssetFile {
public:
	std::string path; //Location of the file
	std::string name; //Name of the file
	std::vector<unsigned char> data;
	int format = FORMAT_FILE_GENERIC;
	bool modified = false; //If file was modified since last saved
	GenericAssetFile(); //Default constructor
	GenericAssetFile(std::vector<unsigned char> fData, std::string name, std::string path = "");
	GenericAssetFile(std::string path); //Load file data from path and identifies its format
};

#endif
