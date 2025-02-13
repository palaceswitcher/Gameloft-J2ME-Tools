#include <SDL3/SDL.h>
#include "PackedFile.hpp"
#include <vector>
#include <memory>
#ifndef FILEVIEW_HPP_ //Include guard
#define FILEVIEW_HPP_

namespace UI {
File* getSelectedFile();
}

// Used to track where a file came from. Used in rendering and drag and drop operations.
struct FileSource {
	std::vector<std::unique_ptr<File>>* fileVector; //Vector that the file belongs to
	std::vector<std::unique_ptr<File>>::iterator fileIter; //Iterator of the file
};

void renderFileView(SDL_Window* window, std::vector<std::unique_ptr<File>>& files);

#endif
