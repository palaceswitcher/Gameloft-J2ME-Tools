#include "PackedFile.hpp"
#include <vector>
#include <memory>
#ifndef FILEMENU_HPP_ //Include guard
#define FILEMENU_HPP_

// The source of a dragged and dropped file.
struct FileSource {
	std::vector<std::unique_ptr<File>>* parentVector; //Vector that the file belongs to
	std::vector<std::unique_ptr<File>>::iterator fileIter; //Iterator of the file
};

void renderPackFileList(SDL_Window* window, std::vector<std::unique_ptr<File>>& files);

#endif
