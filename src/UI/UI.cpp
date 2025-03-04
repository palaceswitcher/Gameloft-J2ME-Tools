#include "UI.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "AssetFile.hpp"
#include "GfxAsset.hpp"
#include "FileDialog.hpp"
#include "FileView.hpp"
#include "GfxView.hpp"
#include <vector>
#include <string>

bool gfxWindowOpen = false;

void openGfxWindow() {
	gfxWindowOpen = true;
}

bool UI::render(SDL_Renderer* ren, SDL_Window* window, std::vector<std::unique_ptr<GenericAssetFile>> &files) {
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
					SDL_ShowOpenFileDialog(fileOpenCallback, &files, window, NULL, 0, NULL, true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// File List
		renderFileView(window, files);

		ImGui::End();
	}
	
	//TODO FINISH ASSET VIEW STUFF
	GenericAssetFile* selectedFile = UI::getSelectedFile();
	if (selectedFile != nullptr) {
		if (selectedFile->format == FORMAT_FILE_GFX) {
			GfxAsset* gfxFile = static_cast<GfxAsset*>(selectedFile);
			UI::addToGfxWindow(gfxFile, ren);
		}
	}
	renderGfxWindow(window, gfxWindowOpen);
	return true;
}
