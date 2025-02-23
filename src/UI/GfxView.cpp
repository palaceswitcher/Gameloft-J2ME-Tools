#include "GfxView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <string>

std::vector<GenericAssetFile*> openedGfxFiles;

void addToGfxWindow(GenericAssetFile* file) {
	openedGfxFiles.push_back(file);
}

void renderGfxWindow(SDL_Window* window, bool &opened) {
	SDL_Renderer* ren = SDL_GetRenderer(window); //Get renderer from window

	//TODO FINISH IMPLEMENTING THIS
	/*if (ImGui::Begin("GFX View", &opened)) {
		if (openedFile->format == FORMAT_FILE_GFX) {
			openedGfxFiles.push_back(openedFile);
			GfxAsset* gfxFile = static_cast<GfxAsset*>(openedFile);
			J2MEImage sprite = gfxFile->gfx.getSprite(0, 1);
			std::vector<int> imgData = sprite.data;
			SDL_Surface* surf = SDL_CreateSurfaceFrom(sprite.width, sprite.height, SDL_PIXELFORMAT_ARGB8888, imgData.data(), sprite.width*4);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf);
			ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(sprite.width, sprite.height));
			//SDL_RenderTexture(ren, texture, NULL, NULL);
		}
	}
	ImGui::End();*/
}
