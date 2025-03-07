#ifndef FILEVIEW_HPP_
#define FILEVIEW_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "AssetPack.hpp"
#include <vector>
#include <memory>

namespace UI {
// Pop last selected file, can only be done once after a file is selected, otherwise it will return a nullptr.
GenericAssetFile* getSelectedFile();
}

// Used to track where a file came from. Used in rendering and drag and drop operations.
struct FileSource {
	std::vector<std::unique_ptr<GenericAssetFile>>* fileVector; //Vector that the file belongs to
	std::vector<std::unique_ptr<GenericAssetFile>>::iterator fileIter; //Iterator of the file
	AssetPack* parentFile = nullptr; //Asset pack the file belongs to, if applicable
};

void renderFileView(SDL_Window* window, std::vector<std::unique_ptr<GenericAssetFile>>& files);

#endif
