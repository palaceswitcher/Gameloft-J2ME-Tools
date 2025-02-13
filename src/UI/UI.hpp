#include <SDL3/SDL.h>
#include "PackedFile.hpp"
#include <vector>
#include <memory>
#ifndef UI_HPP_ //Include guard
#define UI_HPP_

namespace UI {
bool render(SDL_Window* window, std::vector<std::unique_ptr<File>> &files);
}

#endif
