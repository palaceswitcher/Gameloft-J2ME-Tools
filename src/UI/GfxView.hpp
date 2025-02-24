#ifndef GFXVIEW_HPP_
#define GFXVIEW_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>

// Add a file to the list of loaded graphics
namespace UI {
/**
 * Adds a gfx file to the gfx window and returns the index of the 
 */
int addToGfxWindow(GfxAsset* file);
}

// Render the graphics view window.
void renderGfxWindow(SDL_Window* window, bool& opened);

#endif
