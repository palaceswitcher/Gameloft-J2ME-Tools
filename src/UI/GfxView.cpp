#include "GfxView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

std::vector<GfxAsset*> openedGfxFiles;

int UI::addToGfxWindow(GfxAsset* file) {
	if (std::find(openedGfxFiles.begin(), openedGfxFiles.end(), file) == openedGfxFiles.end()) {
		openedGfxFiles.push_back(file);
	}
	return 0;
}

void renderSprites(GfxAsset* gfx, SDL_Renderer* ren) {
	int spriteCount = gfx->gfx.sprites[0].size();
	for (int i = 0; i < spriteCount; i++) {
		J2MEImage sprite = gfx->gfx.getSprite(0, i);
		std::vector<int> imgData = sprite.data;
		SDL_Surface* surf = SDL_CreateSurfaceFrom(sprite.width, sprite.height, SDL_PIXELFORMAT_ARGB8888, imgData.data(), sprite.width*4);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf);
		ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(sprite.width, sprite.height));
	}
	
}

void renderGfxWindow(SDL_Window* window, bool &opened) {
	SDL_Renderer* ren = SDL_GetRenderer(window); //Get renderer from window

	float textureBoxSize = 32.0f;
	int maxImgGridCols = 16;
	int tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_DrawSelectedOverline;
	if (ImGui::Begin("GFX View", &opened)) {
		float windowWidth = ImGui::GetWindowSize().y;
		int colCount = windowWidth / textureBoxSize;
		if (ImGui::BeginTabBar("GfxViewTabs", tabFlags)) {
			for (auto file : openedGfxFiles) {
				if (ImGui::BeginTabItem(file->name.c_str(), NULL, 0)) {
					//for (int r = 0; r < 1; r++) {
						//for (int c = 0; c < colCount; c++) {
							//if (c > 0) { ImGui::SameLine(); }
							renderSprites(file, ren);
						//}
					//}
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
