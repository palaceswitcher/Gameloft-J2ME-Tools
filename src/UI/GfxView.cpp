#include "GfxView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include "GameloftGfx.hpp"
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>

#include <fstream>

auto getCurTimeMs() {
	auto now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

struct OpenedGfxAsset {
	int selectedSpriteIndex; // Index of the selected sprit
	int selPalette = 0;	// Selected palette
	GfxAsset* gfxFile;
	std::int64_t lastFrameTime = 0; // Unix time of last frame in milliseconds, used to render animations
	int currentAnimFrame = 0; // Animation frame currently being drawn
	std::vector<std::vector<SDL_Texture*>> textureBuf; // Sprite texture buffer
	int selAnim = 0;
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
		openedGfxAsset.lastFrameTime = getCurTimeMs();
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
	bool waitingFileRemove = false;	// Whether or not a file is queued for removal
	bool curFileOpen = true; // File is assumed to be kept open until closure
	SDL_Renderer* ren = SDL_GetRenderer(window); // Get renderer from window

	float textureBoxSize = 32.0f;
	int maxImgGridCols = 16;
	int tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_DrawSelectedOverline;
	if (ImGui::Begin("GFX View", &opened)) {
		float windowWidth = ImGui::GetWindowWidth();
		if (ImGui::BeginTabBar("GfxViewTabs", tabFlags)) {
			int i = 0;
			for (auto& file : openedGfxFiles) {
				GfxAsset* gfxFile = file.gfxFile;
				GameloftGraphics glGfx = gfxFile->gfx; // ASprite graphics for the current file

				ImGui::PushID(i);
				bool tabOpened = ImGui::BeginTabItem(gfxFile->name.c_str(), &curFileOpen, 0);
				ImGui::PopID();
				if (tabOpened) {
					if (ImGui::BeginTabBar("GfxViewTabBar")) {
						// Animations tab
						if (ImGui::BeginTabItem("Animations")) {
							std::vector<std::string> animStrings;
							for (int j = 0; j < glGfx.animations.size(); j++) {
								animStrings.push_back("#"+std::to_string(j));
							}
							// Render animation combo
							char* previewString;
							if (file.selAnim >= animStrings.size()) {
								previewString = NULL;
							} else {
								previewString = (char*)animStrings[file.selAnim].c_str();
							}
							if (ImGui::BeginCombo("Animation", previewString)) {
								for (int j = 0; j < animStrings.size(); j++) {
									ImGui::PushID(j);
									if (ImGui::Selectable(animStrings[j].c_str())) {
										file.selAnim = j;
									}
									ImGui::PopID();
								}
								ImGui::EndCombo();
							}
							ImGui::EndTabItem();
						}
						

						std::vector<ASprite::FrameModule> frameModules;
						if (file.selAnim < glGfx.animations.size()) {
							int ind = glGfx.animations[file.selAnim].frameIndex;
							int frameInd = glGfx.animationFrames[ind+file.currentAnimFrame].index; // Current frame number
							ASprite::AnimationFrame animFrame = glGfx.animationFrames[ind+file.currentAnimFrame]; // Current animation frame
							ASprite::Frame frame = glGfx.frames[frameInd]; // Current frame
							ASprite::FrameRect frameRect = glGfx.frameRects[frameInd]; // Current frame's rectangle

							if (getCurTimeMs() - file.lastFrameTime > animFrame.duration * 50) {
								file.currentAnimFrame++; // Go to next frame
								if (file.currentAnimFrame >= glGfx.animations[file.selAnim].frameCount) {
									file.currentAnimFrame = 0; // Loop when last frame is reached
								}
								file.lastFrameTime = getCurTimeMs();
							}

							frameModules = std::vector<ASprite::FrameModule>(glGfx.frameModules.begin()+frame.frameModuleIndex, glGfx.frameModules.begin()+frame.frameModuleIndex+frame.numModules);
							
							ImGui::Selectable("##DisplayedAnim", true, 0, ImVec2(frameRect.w, frameRect.h)); // Animation bounding box (TODO: FIX HOW THIS IS SCALED)

							float xOffs = animFrame.xOffs;
							float yOffs = animFrame.yOffs;
							for (auto &frameMod : frameModules) {
								ASprite::Module module = glGfx.modules[frameMod.modIndex];
								float width = module.w;
								float height = module.h;

								SDL_Texture* texture = file.textureBuf[file.selPalette][frameMod.modIndex];
								ImVec2 drawPos = ImGui::GetItemRectMin();
								// Apply mirroring
								ImVec2 uvMin = ImVec2(0.0f, 0.0f);
								ImVec2 uvMax = ImVec2(1.0f, 1.0f);
								if ((frameMod.flags & 0b01) != 0) {
									uvMin.x = 1.0f;
									uvMax.x = 0.0f;
								}
								if ((frameMod.flags & 0b10) != 0) {
									uvMin.y = 1.0f;
									uvMax.y = 0.0f;
								}
								ImDrawList *drawList = ImGui::GetWindowDrawList();
								drawList->AddImage((ImTextureID)(intptr_t)texture,
								ImVec2(xOffs + drawPos.x + frameMod.x, yOffs + drawPos.y + frameMod.y),
								ImVec2(xOffs + drawPos.x + frameMod.x + width, yOffs + drawPos.y + frameMod.y + height), uvMin, uvMax);
							}
						}

						ImGui::EndTabBar();
					}
					ImGui::EndTabItem();
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
