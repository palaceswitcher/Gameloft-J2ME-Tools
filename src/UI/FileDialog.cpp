#include "FileDialog.hpp"
#include "PackedFile.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <SDL3/SDL.h>

static const SDL_DialogFileFilter filters[] = {
	{"All files", "*"}
};

void SDLCALL fileOpenCallback(void* pFilesOpened, const char* const* fileList, int filter) {
	std::vector<std::unique_ptr<File>>* filesOpened = reinterpret_cast<std::vector<std::unique_ptr<File>>*>(pFilesOpened); //Get vector pointer back
	std::vector<std::string> files;
	if (!fileList) {
		SDL_Log("An error occured: %s", SDL_GetError());
	} else if (!*fileList) {
		SDL_Log("No files selected.");
		return;
	}

	while (*fileList) {
		files.push_back(*fileList);
		filesOpened->push_back(std::make_unique<PackFile>(PackFile(*fileList)));
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