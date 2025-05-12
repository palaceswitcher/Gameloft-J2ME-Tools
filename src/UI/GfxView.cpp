#include "GfxView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include "GameloftGfx.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>

#include <fstream>

struct OpenedGfxAsset {
	int selectedSpriteIndex; //Index of the selected sprit
	int selPalette = 0;	//Selected palette
	GfxAsset* gfxFile;
	std::vector<std::vector<SDL_Texture*>> textureBuf; //Sprite texture buffere
};

OpenedGfxAsset *removedFile;
std::vector<OpenedGfxAsset> openedGfxFiles;

void refreshTextureBuf(std::vector<std::vector<SDL_Texture*>> &textureBuf, GfxAsset* gfxAsset, SDL_Renderer* ren) {
	for (auto& textVec : textureBuf) {
		for (auto& texture : textVec) {
			SDL_DestroyTexture(texture);
		}
	}
	textureBuf.clear();
	gfxAsset->updateSpriteImages();
	for (int i = 0; i < gfxAsset->gfx.sprites.size(); i++) {
		std::vector<SDL_Texture*> textures;
		for (int j = 0; j < gfxAsset->gfx.sprites[i].size(); j++) {
			//std::ofstream outfile("a"+std::to_string(j), std::ios::binary);
			J2MEImage sprite = gfxAsset->gfx.sprites[i][j];
			//outfile.write((char*)sprite.data.data(), sprite.data.size());
			//outfile.close();
			SDL_Surface* surf = SDL_CreateSurfaceFrom(sprite.width, sprite.height, SDL_PIXELFORMAT_ARGB8888, sprite.data.data(), sprite.width*4);
			SDL_Surface* surf2 = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf2);
			SDL_DestroySurface(surf);
			SDL_DestroySurface(surf2);
			textures.push_back(texture);
		}
		textureBuf.push_back(textures);
	}
}

void UI::GfxView::add(GfxAsset* file, SDL_Renderer* ren) {
	auto it = std::find_if(openedGfxFiles.begin(), openedGfxFiles.end(), [&file](const OpenedGfxAsset &g) {
		return g.gfxFile == file;
	});
	if (it == openedGfxFiles.end()) {
		OpenedGfxAsset openedGfxAsset = {0, 0, file};
		refreshTextureBuf(openedGfxAsset.textureBuf, file, ren);
		openedGfxFiles.push_back(openedGfxAsset);
	} else {
		return;
	}
}

void UI::GfxView::remove(GfxAsset* file) {
	if (!openedGfxFiles.empty()) {
		openedGfxFiles.erase(std::remove_if(openedGfxFiles.begin(), openedGfxFiles.end(), [file](const OpenedGfxAsset &g) {
			return g.gfxFile == file;
		}));
	}
}

void renderGfxWindow(SDL_Window* window, bool& opened) {
	bool waitingFileRemove = false;	//Whether or not a file is queued for removal
	bool curFileOpen = true; //File is assumed to be kept open until closure
	SDL_Renderer* ren = SDL_GetRenderer(window); //Get renderer from window

	float textureBoxSize = 32.0f;
	int maxImgGridCols = 16;
	int tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_DrawSelectedOverline;
	if (ImGui::Begin("GFX View", &opened)) {
		float windowWidth = ImGui::GetWindowWidth();
		if (ImGui::BeginTabBar("GfxViewTabs", tabFlags)) {
			int i = 0;
			for (auto& file : openedGfxFiles) {
				GfxAsset* gfxFile = file.gfxFile;

				ImGui::PushID(i);
				bool tabOpened = ImGui::BeginTabItem(gfxFile->name.c_str(), &curFileOpen, 0);
				ImGui::PopID();
				if (tabOpened) {
					int spriteCount = gfxFile->gfx.sprites[file.selPalette].size(); //Number of sprites for the currently viewed file
					// Palette change
					if (ImGui::InputInt("Palette", &file.selPalette)) {
						if (file.selPalette >= file.textureBuf.size()) {
							file.selPalette = file.textureBuf.size() - 1;
						}
					}
					// Palette show
					if (ImGui::TreeNode("Palettes")) {
						int n = 0;
						for (int p = 0; p < file.gfxFile->gfx.palettes.size(); p++) {
							auto palette = file.gfxFile->gfx.palettes[p];
							if (!palette.empty()) {
								ImGui::Text("#%d", p);
								ImGui::SameLine();
								for (int c = 0; c < palette.size(); c++) {
									int color = palette[c];
									float a = ((color >> 24) & 0xFF) / 255.0f;
									float r = ((color >> 16) & 0xFF) / 255.0f;
									float g = ((color >> 8) & 0xFF) / 255.0f;
									float b = (color & 0xFF) / 255.0f; //Convert ARGB color to floats
									std::vector<float> colVec = {r, g, b, a};
									if (c != 0) {
										ImGui::SameLine();
									}
									ImGui::PushID(n);
									if (ImGui::ColorButton("##", ImVec4(r, g, b, a))) {
										ImGui::OpenPopup("ColorPickerPopup");
									}
									if (ImGui::BeginPopup("ColorPickerPopup")) {
										if (ImGui::ColorPicker4("Edit Color", &colVec[0])) {
											int newCol = (int)(255.0f * colVec[3]) << 24 |
														 (int)(255.0f * colVec[0]) << 16 |
														 (int)(255.0f * colVec[1]) << 8 |
														 (int)(255.0f * colVec[2]); //Convert floats back to ARGB
											file.gfxFile->gfx.palettes[p][c] = newCol;
											refreshTextureBuf(file.textureBuf, file.gfxFile, ren);
										}
										ImGui::EndPopup();
									}
									ImGui::PopID();
									n++;
								}
							}
						}
						ImGui::TreePop();
					}
					int colCount = floorf(ImGui::GetWindowWidth() / (textureBoxSize + ImGui::GetStyle().WindowPadding.x)); // Number of columns
					if (colCount <= 0) {
						colCount = 1;
					}
					for (int sp = 0; sp < spriteCount; sp++) {
						if (sp % colCount != 0) {
							ImGui::SameLine();
						}

						SDL_Texture* texture = file.textureBuf[file.selPalette][sp];
						SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
						ImGui::PushID(sp);
						if (ImGui::Selectable("##", true, 0, ImVec2(textureBoxSize, textureBoxSize))) {
							file.selectedSpriteIndex = sp;
						}
						if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
							ImGui::OpenPopup("SpriteViewPopup");
						}
						if (ImGui::BeginPopup("SpriteViewPopup")) {
							if (ImGui::MenuItem("Save as PNG")) {
								file.gfxFile->exportSprite(sp, file.selPalette);
							}
							ImGui::EndPopup();
						}

						ImGui::PopID();

						float spriteDrawWidth = texture->w;
						float spriteDrawHeight = texture->h;
						ImVec2 max = ImGui::GetItemRectMax();
						ImVec2 min = ImGui::GetItemRectMin();
						float boxWidth = max.x - min.x;
						float boxHeight = max.y - min.y;
	
						if (spriteDrawWidth > boxWidth) {
							spriteDrawHeight *= boxWidth / spriteDrawWidth;
							spriteDrawWidth = boxWidth;
						}
						if (spriteDrawHeight > boxHeight) {
							spriteDrawWidth *= boxHeight / spriteDrawHeight;
							spriteDrawHeight = boxHeight;
						}

						ImVec2 center = ImVec2(ceilf(min.x + ((boxWidth - spriteDrawWidth) * 0.5f)), min.y + ceilf(((boxHeight - spriteDrawHeight) * 0.5f)));
						ImDrawList *drawList = ImGui::GetWindowDrawList();
						drawList->AddImage((ImTextureID)(intptr_t)texture, center, ImVec2(spriteDrawWidth + center.x, spriteDrawHeight + center.y));
					}
					ImGui::EndTabItem();

					SDL_Texture* selectedSpriteTexture = file.textureBuf[file.selPalette][file.selectedSpriteIndex];
					SDL_SetTextureScaleMode(selectedSpriteTexture, SDL_SCALEMODE_NEAREST);
					ImGui::Image((ImTextureID)(intptr_t)selectedSpriteTexture, ImVec2(
								selectedSpriteTexture->w*2,
								selectedSpriteTexture->h*2));
				}
				if (!curFileOpen) {
					removedFile = &file;
					waitingFileRemove = true;
					curFileOpen = true;
				}
				i++;
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
	if (waitingFileRemove) {
		openedGfxFiles.erase(std::remove_if(openedGfxFiles.begin(), openedGfxFiles.end(), [](const OpenedGfxAsset &g) {
			return &g == removedFile;
		}));
		waitingFileRemove = false; //File is no longer queued for removal
		curFileOpen = true;
	}
}
