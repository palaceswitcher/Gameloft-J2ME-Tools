#ifndef GFXVIEW_HPP_
#define GFXVIEW_HPP_
#include <SDL3/SDL.h>
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <memory>

// Add a file to the list of loaded graphics
void addToGfxWindow(GenericAssetFile* file);

// Render the graphics view window.
void renderGfxWindow(SDL_Window* window, bool& opened);

#endif
