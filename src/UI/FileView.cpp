#include "FileView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "FileDialog.hpp"
#include "AssetFile.hpp"
#include "AssetPack.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

std::vector<std::unique_ptr<GenericAssetFile>> openedFiles; //Every opened file
GenericAssetFile* selectedFile = nullptr;
FileSource rightClickedFile; //File with an opened context menu
std::vector<FileSource> fileRemovalQueue; //Any files that were queued for removal

std::vector<std::unique_ptr<GenericAssetFile>>* UI::FileMenu::getFiles() {
	return &openedFiles;
}

GenericAssetFile* UI::FileMenu::popSelectedFile() {
	GenericAssetFile* ret = selectedFile;
	selectedFile = nullptr;
	return ret;
}

std::vector<FileSource> UI::FileMenu::popRemovedFiles() {
	std::vector<FileSource> removedFiles = fileRemovalQueue;
	fileRemovalQueue.clear();
	return removedFiles;
}

void UI::FileMenu::remove(FileSource fileSrc) {
	GenericAssetFile* file = (*fileSrc.fileIter).get();
	std::vector<std::unique_ptr<GenericAssetFile>>* fileVec = fileSrc.fileVector;
	fileVec->erase(std::remove_if(fileVec->begin(), fileVec->end(), [file](const std::unique_ptr<GenericAssetFile> &g) {
		return g.get() == file;
	}));
}

// Renders the popup context menu for a generic file, only to be used in file menu.
void enableFileContextPopup(SDL_Window* window) {
	GenericAssetFile* file;
	if ((rightClickedFile.fileVector) == nullptr) {
		return;
	} else {
		file = (*rightClickedFile.fileIter).get();
	}
	if (ImGui::BeginPopup("FileContextPopup")) {
		if (ImGui::MenuItem("Export")) {
			SDL_ShowSaveFileDialog(fileSaveCallback, &(file->data), window, NULL, 0, NULL);
		}
		if (ImGui::MenuItem("Remove")) {
			fileRemovalQueue.push_back(rightClickedFile);
		}
		ImGui::EndPopup();
	}
}

// Return true if the objects are the same
bool isDescendant(const GenericAssetFile* file, const GenericAssetFile* srcFile) {
	if (file == srcFile) {
		return true; //Failsafe if they are somehow the same object
	}
	if (file->format >= FORMAT_PK_OFFS) {
		const AssetPack* packFile = static_cast<const AssetPack*>(file);
		for (const auto& subFile : packFile->subFiles) {
			if (isDescendant(subFile.get(), srcFile)) {
				return true;
			}
		}
	}
	return false;
}

/**
 * Renders a file.
 * @param fileSrc Location of the file being rendered
 * @param indentWidth width of the file's indentation
 */
void renderFile(FileSource fileSrc, SDL_Window* window, float indentWidth, int& index) {
	// Pack file specific variables (if applicable)
	AssetPack* packFile = nullptr; //Pack file being rendered
	GenericAssetFile* file = (*fileSrc.fileIter).get(); //The file being rendered
	bool treeNodeOpen = false;

	std::string name = file->name;
	ImGui::PushID(index); //Index-based unique IDs
	if (file->format < FORMAT_PK_OFFS) {
		if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_None)) {
			selectedFile = file; //Select the clicked file
		}
	} else {
		packFile = static_cast<AssetPack*>(file);
		treeNodeOpen = ImGui::TreeNodeEx(packFile->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow); //Cache pack tree node open state for asset packs
	}
	ImGui::PopID(); //Pop ID
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
		rightClickedFile = fileSrc;
		ImGui::OpenPopup("FileContextPopup");
	}
	// Drag and drop source for file
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("FILE_ITEM", &fileSrc, sizeof(fileSrc));
		ImGui::EndDragDropSource();
	}

	// Rendering for pack files
	if (treeNodeOpen) {
		// Drop zone at top of asset pack files
		float fileYPos = ImGui::GetCursorPosY();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
		ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
				auto destIt = packFile->subFiles.begin(); //Insert point for this specific target
				FileSource* droppedFileSrc = (FileSource*)payload->Data; //File that was dropped
				GenericAssetFile* droppedFile = (*droppedFileSrc->fileIter).get();
				bool canBeDropped = true; //Used to determine if the file can be dropped. Assumed to be true if the file is not an asset pack
				if (droppedFile->format >= FORMAT_PK_OFFS) {
					AssetPack* droppedPackFile = static_cast<AssetPack*>(droppedFile);
					canBeDropped = (&(droppedPackFile->subFiles) != fileSrc.fileVector) && !isDescendant(droppedPackFile, file); //The asset pack cannot be dropped into itself
				}
				if (canBeDropped && *destIt != *droppedFileSrc->fileIter) {
					// Move to other vector
					if (droppedFileSrc->fileVector != &packFile->subFiles) {
						packFile->subFiles.insert(destIt, std::move(*(droppedFileSrc->fileIter)));
						droppedFileSrc->fileVector->erase(droppedFileSrc->fileIter);
					// Move within same vector
					} else {
						if (droppedFileSrc->fileIter < packFile->subFiles.begin()) {
							std::rotate(droppedFileSrc->fileIter, droppedFileSrc->fileIter + 1, packFile->subFiles.begin() + 1);
						} else if (droppedFileSrc->fileIter > packFile->subFiles.begin()) {
							std::rotate(destIt, droppedFileSrc->fileIter, droppedFileSrc->fileIter + 1);
						}
					}
					if (droppedFileSrc->parentFile != nullptr) {
						droppedFileSrc->parentFile->refreshData(); //Refresh asset pack the dropped file was moved from if needed
					}
					if (packFile != nullptr) {
						packFile->refreshData(); //Refresh pack file's data if the subfiles were changed
					}
				}
			}
			ImGui::EndDragDropTarget();
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
		}

		for (int j = 0; j < packFile->subFiles.size(); j++) {
			renderFile({&packFile->subFiles, packFile->subFiles.begin() + j, packFile}, window, indentWidth, index);
			index++;
		}
		enableFileContextPopup(window); //Show generic file popup when right clicked
		ImGui::TreePop();
	}

	// Drop zone below the file
	float fileYPos = ImGui::GetCursorPosY();
	ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
			auto destIt = fileSrc.fileIter + 1; //Insert after the element for this point
			FileSource* droppedFileSrc = (FileSource*)payload->Data; //File that was dropped in
			GenericAssetFile* droppedFile = (*droppedFileSrc->fileIter).get();

			bool canBeDropped = true; //Used to determine if the file can be dropped. Assumed to be true if the file is not an asset pack
			if (droppedFile->format >= FORMAT_PK_OFFS) {
				AssetPack* droppedPackFile = static_cast<AssetPack*>(droppedFile);
				canBeDropped = (&(droppedPackFile->subFiles) != fileSrc.fileVector) && !isDescendant(droppedPackFile, file); //The asset pack cannot be dropped into itself
			}
			if (canBeDropped && *(fileSrc.fileIter) != *droppedFileSrc->fileIter) {
				// Insert outside of vector
				if (droppedFileSrc->fileVector != fileSrc.fileVector) {
					fileSrc.fileVector->insert(destIt, std::move(*(droppedFileSrc->fileIter)));
					droppedFileSrc->fileVector->erase(droppedFileSrc->fileIter);
				// Insert within vector
				} else {
					if (destIt >= fileSrc.fileVector->end()) {
						destIt = fileSrc.fileIter; //Don't insert out of bounds if the file is moved within the same vector
					}
					if (droppedFileSrc->fileIter < fileSrc.fileIter+1) {
						std::rotate(droppedFileSrc->fileIter, droppedFileSrc->fileIter + 1, fileSrc.fileIter+1);
					} else if (droppedFileSrc->fileIter > fileSrc.fileIter+1) {
						std::rotate(destIt, droppedFileSrc->fileIter, droppedFileSrc->fileIter+1);
					}
				}
				if (droppedFileSrc->parentFile != nullptr) {
					droppedFileSrc->parentFile->refreshData(); //Refresh asset pack the dropped file was moved from if needed
				}
				if (packFile != nullptr) {
					packFile->refreshData();
				}
			}
		}
		ImGui::EndDragDropTarget();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	}
}

void renderFileView(SDL_Window* window) {
	int index = 0; //Used for file IDs in ImGui
	ImGui::Text("Files"); //Child title
	ImGui::BeginChild("FileViewPanel", ImVec2(250, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_NoMove);
	float indentWidth = 0;
	// Drop zone above the top file
	float fileYPos = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(fileYPos - 6.0f);
	ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - 16.0f, 12.0f));
	ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
			auto destIt = openedFiles.begin(); //Insert point for this specific target
			FileSource* droppedFileSrc = (FileSource*)payload->Data;
			if (*destIt != *droppedFileSrc->fileIter) {
				if (droppedFileSrc->fileVector != &openedFiles) {
					openedFiles.insert(destIt, std::move(*(droppedFileSrc->fileIter)));
					droppedFileSrc->fileVector->erase(droppedFileSrc->fileIter);
				} else {
					if (droppedFileSrc->fileIter < destIt) {
						std::rotate(droppedFileSrc->fileIter, droppedFileSrc->fileIter+1, destIt+1);
					} else if (droppedFileSrc->fileIter > destIt) {
						std::rotate(destIt, droppedFileSrc->fileIter, droppedFileSrc->fileIter+1);
					}
				}
				if (droppedFileSrc->parentFile != nullptr) {
					droppedFileSrc->parentFile->refreshData(); //Refresh asset pack the dropped file was moved from if needed
				}
			}
		}
		ImGui::EndDragDropTarget();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - indentWidth - ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	}
	for (int i = 0; i < openedFiles.size(); i++) {
		renderFile({&openedFiles, openedFiles.begin()+i, nullptr}, window, indentWidth, index);
		index++;
	}
	enableFileContextPopup(window); //Show generic file popup when right clicked
	ImGui::EndChild();
}
