#include "FileView.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "FileDialog.hpp"
#include "PackedFile.hpp"
#include <vector>
#include <string>
#include <algorithm>

File* selectedFile = nullptr;
File* rightClickedFile = nullptr; //File with an opened context menu

File* UI::getSelectedFile() {
	return selectedFile;
}

// Renders the popup context menu for a generic file, only to be used in file menu.
void enableFileContextPopup(SDL_Window* window) {
	if (rightClickedFile == nullptr) {
		return;
	}
	if (ImGui::BeginPopup("FileContextPopup")) {
		if (ImGui::MenuItem("Export")) {
			SDL_ShowSaveFileDialog(fileSaveCallback, &(rightClickedFile->data), window, NULL, 0, NULL);
		}
		ImGui::EndPopup();
	}
}

// Return true if the objects are the same
bool isDescendant(const File* file, const File* srcFile) {
	if (file == srcFile) { return true; } //Failsafe if they are somehow the same object
	if (file->format >= FORMAT_PK_OFFS) {
		const PackFile* packFile = static_cast<const PackFile*>(file);
		for (const auto& subFile : packFile->subFiles) {
			if (isDescendant(subFile.get(), srcFile)) {
				return true;
			}
		}
	}
	return false;
}

/**
 * @private Renders a file.
 * @param fileSrc Location of the file being rendered
 * @param indentWidth width of the file's indentation
 */
void renderFile(FileSource fileSrc, SDL_Window* window, float indentWidth) {
	// Pack file specific variables (if applicable)
	PackFile* packFile; //Pack file being rendered
	File* file = (*fileSrc.fileIter).get(); //The file being rendered
	bool treeNodeOpen = false;
	std::string name = file->name;
	if (file->format < FORMAT_PK_OFFS) {
		if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_None)) {
			selectedFile = file; //Select the clicked file
		}
	} else {
		packFile = static_cast<PackFile*>(file);
		treeNodeOpen = ImGui::TreeNodeEx(packFile->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow); //Cache pack tree node open state
	}
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
		rightClickedFile = file;
		ImGui::OpenPopup("FileContextPopup");
	}
	// Drag and drop source for file
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("FILE_ITEM", &fileSrc, sizeof(fileSrc));
		ImGui::EndDragDropSource();
	}

	// Rendering for pack files
	if (treeNodeOpen) {
		// Drop zone at top of tree
		float fileYPos = ImGui::GetCursorPosY();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
		ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
				auto destIt = packFile->subFiles.begin(); //Insert point for this specific target
				FileSource* fileSrc = (FileSource*)payload->Data;
				if (*destIt != *fileSrc->fileIter) {
					if (fileSrc->fileVector != &packFile->subFiles) {
						packFile->subFiles.insert(destIt, std::move(*(fileSrc->fileIter)));
						fileSrc->fileVector->erase(fileSrc->fileIter);
					} else {
						if (fileSrc->fileIter < destIt) {
							std::rotate(fileSrc->fileIter, fileSrc->fileIter+1, destIt+1);
						} else if (fileSrc->fileIter > destIt) {
							std::rotate(destIt, fileSrc->fileIter, fileSrc->fileIter+1);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
		}

		for (int j = 0; j < packFile->subFiles.size(); j++) {
			renderFile({&packFile->subFiles, packFile->subFiles.begin()+j}, window, indentWidth);
		}
		enableFileContextPopup(window); //Show generic file popup when right clicked
		ImGui::TreePop();
	}

	// Drop zone below the file
	float fileYPos = ImGui::GetCursorPosY();
	ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
			FileSource* droppedFileSrc = (FileSource*)payload->Data; //Get dropped first so we can determine if it can be dropped
			File* droppedFile = (*droppedFileSrc->fileIter).get();

			bool canBeDropped = true; //Used to determine if the file can be dropped
			if (droppedFile->format >= FORMAT_PK_OFFS) {
				PackFile* droppedPackFile = static_cast<PackFile*>(droppedFile);
				canBeDropped = (&(droppedPackFile->subFiles) != fileSrc.fileVector) && !isDescendant(droppedPackFile, file); //The pack file cannot be dropped into itself
			}
			if (canBeDropped) {
				auto destIt = fileSrc.fileIter+1; //Insert after the element for this point
				if (*(fileSrc.fileIter) != *droppedFileSrc->fileIter) {
					if (droppedFileSrc->fileVector != fileSrc.fileVector) {
						fileSrc.fileVector->insert(destIt, std::move(*(droppedFileSrc->fileIter)));
						droppedFileSrc->fileVector->erase(droppedFileSrc->fileIter);
					} else {
						if (fileSrc.fileIter+1 >= fileSrc.fileVector->end()) {
							destIt = fileSrc.fileIter; //Don't insert out of bounds if the file is moved within the same vector
						}
						if (droppedFileSrc->fileIter < destIt) {
							std::rotate(droppedFileSrc->fileIter, droppedFileSrc->fileIter+1, destIt+1);
						} else if (droppedFileSrc->fileIter > destIt) {
							std::rotate(destIt, droppedFileSrc->fileIter, droppedFileSrc->fileIter+1);
						}
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	}
}

void renderFileView(SDL_Window* window, std::vector<std::unique_ptr<File>>& files) {
	ImGui::Text("Files"); //Child title
	ImGui::BeginChild("FileViewPanel", ImVec2(250, 0), ImGuiChildFlags_Borders|ImGuiChildFlags_ResizeX, ImGuiWindowFlags_NoMove);
	float indentWidth = 0;
	// Drop zone for the top file
	float fileYPos = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(fileYPos-6.0f);
	ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-16.0f, 12.0f));
	ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
			auto destIt = files.begin(); //Insert point for this specific target
			FileSource* fileSrc = (FileSource*)payload->Data;
			if (*destIt != *fileSrc->fileIter) {
				if (fileSrc->fileVector != &files) {
					files.insert(destIt, std::move(*(fileSrc->fileIter)));
					fileSrc->fileVector->erase(fileSrc->fileIter);
				} else {
					if (fileSrc->fileIter < destIt) {
						std::rotate(fileSrc->fileIter, fileSrc->fileIter+1, destIt+1);
					} else if (fileSrc->fileIter > destIt) {
						std::rotate(destIt, fileSrc->fileIter, fileSrc->fileIter+1);
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth-ImGui::GetTextLineHeightWithSpacing(), ImGui::GetFontSize()));
	}
	for (int i = 0; i < files.size(); i++) {
		renderFile({&files, files.begin()+i}, window, indentWidth);
		enableFileContextPopup(window); //Show generic file popup when right clicked
	}
	ImGui::EndChild();
}
