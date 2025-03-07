#ifndef FILEVIEW_HPP_
#define FILEVIEW_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "AssetPack.hpp"
#include <vector>
#include <memory>

// Used to track where a file came from. Used in rendering and drag and drop operations.
struct FileSource {
	std::vector<std::unique_ptr<GenericAssetFile>>* fileVector; //Vector that the file belongs to
	std::vector<std::unique_ptr<GenericAssetFile>>::iterator fileIter; //Iterator of the file
	AssetPack* parentFile = nullptr; //Asset pack the file belongs to, if applicable
};

namespace UI {
namespace FileMenu {
// Pop last selected file, can only be done once after a file is selected, otherwise it will return a nullptr.
GenericAssetFile* popSelectedFile();

// Return a reference to the list of opened files
std::vector<std::unique_ptr<GenericAssetFile>>* getFiles();

// Pop the list of files that were queued for removal
std::vector<FileSource> popRemovedFiles();

// Remove file from list of files
void remove(FileSource fileSrc);
}
}

void renderFileView(SDL_Window* window);

#endif
