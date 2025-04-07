#include "UI.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include "FileDialog.hpp"
#include "FileView.hpp"
#include "GfxView.hpp"
#include <iostream>
#include <vector>
#include <string>

bool gfxWindowOpen = false;

void openGfxWindow() {
	gfxWindowOpen = true;
}

bool UI::render(SDL_Renderer* ren, SDL_Window* window) {
	// Main app window
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
	{
		ImGui::Begin("Main Window", nullptr, flags);

		// Menu bar
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open")) {
					SDL_ShowOpenFileDialog(fileOpenCallback, UI::FileMenu::getFiles(), window, NULL, 0, NULL, true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// File List
		renderFileView(window);

		ImGui::End();
	}
	
	// Remove any files that were queued for removal
	std::vector<FileSource> removedFiles = UI::FileMenu::popRemovedFiles();
	for (auto removedFile : removedFiles) {
		GenericAssetFile* file = (*removedFile.fileIter).get();
		if (file != nullptr) {
			if (file->format == FORMAT_FILE_GFX) {
				GfxAsset* gfxFile = static_cast<GfxAsset*>(file);
				UI::GfxView::remove(gfxFile);
			} else if (file->format >= FORMAT_PK_OFFS) {
				AssetPack* assetPack = static_cast<AssetPack*>(file);
				for (auto& subFile : assetPack->subFiles) {
					if (subFile->format == FORMAT_FILE_GFX) {
						GfxAsset* gfxFile = static_cast<GfxAsset*>(subFile.get());
						UI::GfxView::remove(gfxFile);
					}
				}
			}
			UI::FileMenu::remove(removedFile);
		}
	}

	GenericAssetFile* selectedFile = UI::FileMenu::popSelectedFile();
	if (selectedFile != nullptr) {
		if (selectedFile->format == FORMAT_FILE_GFX) {
			GfxAsset* gfxFile = static_cast<GfxAsset*>(selectedFile);
			UI::GfxView::add(gfxFile, ren);
		}
	}
	renderGfxWindow(window, gfxWindowOpen);
	return true;
}
