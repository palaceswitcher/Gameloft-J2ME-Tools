#include "GfxView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>

std::vector<GfxAsset*> openedGfxFiles;

int UI::addToGfxWindow(GfxAsset* file) {
	if (std::find(openedGfxFiles.begin(), openedGfxFiles.end(), file) == openedGfxFiles.end()) {
		openedGfxFiles.push_back(file);
	}
	return 0;
}

void renderGfxWindow(SDL_Window* window, bool &opened) {
	SDL_Renderer* ren = SDL_GetRenderer(window); //Get renderer from window

	float textureBoxSize = 32.0f;
	int maxImgGridCols = 16;
	int tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_DrawSelectedOverline;
	if (ImGui::Begin("GFX View", &opened)) {
		float windowWidth = ImGui::GetWindowSize().x;
		if (ImGui::BeginTabBar("GfxViewTabs", tabFlags)) {
			for (auto file : openedGfxFiles) {
				if (ImGui::BeginTabItem(file->name.c_str(), NULL, 0)) {
					int spriteCount = file->gfx.sprites[0].size();
					int colCount = windowWidth / textureBoxSize; //Number of columns
					if (colCount <= 0) { colCount = 1; }
					int rowCount = spriteCount / colCount; //Number of rows
					for (int sp = 0; sp < spriteCount; sp++) {
						if (sp % colCount != 0) { ImGui::SameLine(); }
						J2MEImage sprite = file->gfx.getSprite(0, sp);
						std::vector<int> imgData = sprite.data;
						SDL_Surface* surf = SDL_CreateSurfaceFrom(sprite.width, sprite.height, SDL_PIXELFORMAT_ARGB8888, imgData.data(), sprite.width*4);

						SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf);
						SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
						ImGui::PushID(sp);
						ImGui::Selectable("##", true, 0, ImVec2(textureBoxSize, textureBoxSize));
						ImGui::PopID();

						float spriteDrawWidth = sprite.width;
						float spriteDrawHeight = sprite.height;
						ImVec2 min = ImGui::GetItemRectMax();
						ImVec2 max = ImGui::GetItemRectMin();
						ImVec2 center = ImVec2(min.x + ceil((max.x - min.x - spriteDrawWidth) * 0.5f), min.y + floor((max.y - min.y - spriteDrawHeight) * 0.5f));
						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						draw_list->AddImage((ImTextureID)(intptr_t)texture, center, ImVec2(center.x+spriteDrawWidth, center.y+spriteDrawHeight));
					}
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
