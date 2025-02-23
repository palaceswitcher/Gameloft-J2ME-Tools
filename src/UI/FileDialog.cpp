#include "FileDialog.hpp"
#include "AssetFile.hpp"
#include "AssetManager.hpp"
#include <fstream>
#include <iterator>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <SDL3/SDL.h>

static const SDL_DialogFileFilter filters[] = {
	{"All files", "*"}
};

void SDLCALL fileOpenCallback(void* pFilesOpened, const char* const* fileList, int filter) {
	std::vector<std::unique_ptr<GenericAssetFile>>* filesOpened = reinterpret_cast<std::vector<std::unique_ptr<GenericAssetFile>>*>(pFilesOpened); //Get vector pointer back
	std::vector<std::string> files;
	if (!fileList) {
		SDL_Log("An error occured: %s", SDL_GetError());
	} else if (!*fileList) {
		SDL_Log("No files selected.");
		return;
	}

	while (*fileList) {
		std::string path = *fileList;
		files.push_back(path);
		// Load file from path
		std::vector<unsigned char> data;
		std::ifstream inFile(path, std::ifstream::binary|std::ifstream::ate);
		if (!inFile.fail()) {
			inFile >> std::noskipws; //This is a binary file
			data.reserve(inFile.tellg());
			inFile.seekg(0, std::ios::beg);
			std::copy(std::istream_iterator<unsigned char>(inFile), std::istream_iterator<unsigned char>(), std::back_inserter(data)); //Load file data into vector
			std::string name = path.substr(path.find_last_of("/\\") + 1); //Set path and name
			filesOpened->push_back(loadAsset(data, name, path));
		} else {
			std::cerr << "Error loading file: " << path << '\n';
		}
		fileList++;
	}
}

void SDLCALL fileSaveCallback(void* pfileData, const char* const* fileList, int filter) {
	std::vector<char>* fileData = reinterpret_cast<std::vector<char>*>(pfileData);
	if (!fileList) {
		SDL_Log("An error occured: %s", SDL_GetError());
	} else if (!*fileList) {
		SDL_Log("No files selected.");
		return;
	}
	std::ofstream outFile(*fileList, std::ios::out|std::ios::binary);
	outFile.write(fileData->data(), fileData->size()); //Write saved file
}