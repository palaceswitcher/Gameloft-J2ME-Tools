#ifndef GFXVIEW_HPP_
#define GFXVIEW_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>

// Add a file to the list of loaded graphics
namespace UI {
namespace GfxView {

// Adds a GFX file to the GFX window
void add(GfxAsset* file, SDL_Renderer* ren);

// Remove a file from the GFX view
void remove(GfxAsset* file);
}
}

// Render the graphics view window.
void renderGfxWindow(SDL_Window* window, bool& opened);

#endif
