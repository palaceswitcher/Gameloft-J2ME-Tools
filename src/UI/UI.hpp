#ifndef UI_HPP_
#define UI_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include <vector>
#include <memory>

namespace UI {
bool render(SDL_Renderer* ren, SDL_Window* window);
}

#endif
