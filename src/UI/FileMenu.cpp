#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "FileDialog.hpp"
#include "FileMenu.hpp"
#include "PackedFile.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

File* selectedFile = nullptr;
File* rightClickedFile = nullptr; //File with an opened context menu

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

void renderPackFileList(SDL_Window* window, std::vector<std::unique_ptr<File>>& files) {
	ImGui::BeginChild("FileViewPanel", ImVec2(250, 0), ImGuiChildFlags_Borders|ImGuiChildFlags_ResizeX, ImGuiWindowFlags_NoMove);
	float indentWidth = 0;
	for (int i = 0; i < files.size(); i++) {
		// Drop zone for the top file
		float fileYPos = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(fileYPos-6.0f);
		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth, 12.0f));
		ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
				auto destIt = files.begin()+i; //Insert point for this specific target
				FileSource* fileSrc = (FileSource*)payload->Data;
				if (*(files.begin()+i) != *fileSrc->fileIter) {
					if (fileSrc->parentVector != &files) {
						files.insert(destIt, std::move(*(fileSrc->fileIter)));
						fileSrc->parentVector->erase(fileSrc->fileIter);
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
		}
		if (files[i]->format >= FORMAT_PK_OFFS) {
			PackFile* packFile = static_cast<PackFile*>(files[i].get());
			if (ImGui::TreeNode(packFile->name.c_str())) {
				indentWidth += ImGui::GetTreeNodeToLabelSpacing();
				// Drop zone above only the first file
				float fileYPos = ImGui::GetCursorPosY();
				ImGui::SetCursorPosY(fileYPos-6.0f);
				ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth, 12.0f));
				ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
						auto destIt = packFile->subFiles.begin(); //Insert point for this specific target
						FileSource* fileSrc = (FileSource*)payload->Data;
						if (*(packFile->subFiles.begin()) != *fileSrc->fileIter) {
							if (fileSrc->parentVector != &packFile->subFiles) {
								packFile->subFiles.insert(destIt, std::move(*(fileSrc->fileIter)));
								fileSrc->parentVector->erase(fileSrc->fileIter);
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
				}
				for (int j = 0; j < packFile->subFiles.size(); j++) {
					std::string name = packFile->subFiles[j]->name;
					if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_None)) {
						selectedFile = packFile->subFiles[j].get(); //Select the clicked file
					}
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
						rightClickedFile = packFile->subFiles[j].get();
						ImGui::OpenPopup("FileContextPopup");
					}
					// Drag and drop source for file
					if (ImGui::BeginDragDropSource()) {
						FileSource fileSrc = {&packFile->subFiles, packFile->subFiles.begin()+j};
						ImGui::SetDragDropPayload("FILE_ITEM", &fileSrc, sizeof(fileSrc));
						ImGui::EndDragDropSource();
					} 
					// Drag zone below the file
					float fileYPos = ImGui::GetCursorPosY();
					ImGui::SetCursorPosY(fileYPos-6.0f);
					ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth, 12.0f));
					ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
							auto destIt = packFile->subFiles.begin()+j+1; //Insert after the element for this point
							FileSource* fileSrc = (FileSource*)payload->Data;
							if (*(packFile->subFiles.begin()+j) != *fileSrc->fileIter) {
								if (fileSrc->parentVector != &packFile->subFiles) {
									packFile->subFiles.insert(destIt, std::move(*(fileSrc->fileIter)));
									fileSrc->parentVector->erase(fileSrc->fileIter);
								} else {
									if (j+1 >= packFile->subFiles.size()) {
										destIt = packFile->subFiles.begin()+j; //Don't insert out of bounds if the file is moved within the same vector
									}
									if (fileSrc->fileIter < destIt) {
										std::rotate(fileSrc->fileIter, fileSrc->fileIter+1, destIt+1);
									} else if (fileSrc->fileIter > destIt) {
										std::rotate(destIt, fileSrc->fileIter, fileSrc->fileIter+1);
									}
								}
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
				enableFileContextPopup(window); //Show generic file popup when right clicked
				ImGui::TreePop();
			}
			indentWidth = 0;
			// Drag and drop below the packed file, to drag items out of it
			fileYPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(fileYPos-6.0f);
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth, 12.0f));
			ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
					auto destIt = files.begin()+i+1; //Insert after the element for this point
					FileSource* fileSrc = (FileSource*)payload->Data;
					if (*(files.begin()+i) != *fileSrc->fileIter) {
						if (fileSrc->parentVector != &files) {
							files.insert(destIt, std::move(*(fileSrc->fileIter)));
							fileSrc->parentVector->erase(fileSrc->fileIter);
						} else {
							if (i+1 >= files.size()) {
								destIt = files.begin()+i; //Don't insert out of bounds if the file is within the main file vector
							}
							if (fileSrc->fileIter < destIt) {
								std::rotate(fileSrc->fileIter, fileSrc->fileIter+1, destIt+1);
							} else if (fileSrc->fileIter > destIt) {
								std::rotate(destIt, fileSrc->fileIter, fileSrc->fileIter+1);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		} else if (files[i]->format < FORMAT_PK_OFFS) {
			std::string name = files[i]->name;
			if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_None)) {
				selectedFile = files[i].get(); //Select the clicked file
					}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				rightClickedFile = files[i].get();
				ImGui::OpenPopup("FileContextPopup");
			}
			// Drag and drop source for file
			if (ImGui::BeginDragDropSource()) {
				FileSource fileSrc = {&files, files.begin()+i};
				ImGui::SetDragDropPayload("FILE_ITEM", &fileSrc, sizeof(fileSrc));
				ImGui::EndDragDropSource();
			} 
			// Drag zone below the file
			float fileYPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(fileYPos-6.0f);
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-indentWidth, 12.0f));
			ImGui::SetCursorPosY(fileYPos); //Draw dummy drop zone between files
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_ITEM")) {
					auto destIt = files.begin()+i+1; //Insert after the element for this point
					FileSource* fileSrc = (FileSource*)payload->Data;
					if (*(files.begin()+i) != *fileSrc->fileIter) {
						if (fileSrc->parentVector != &files) {
							files.insert(destIt, std::move(*(fileSrc->fileIter)));
							fileSrc->parentVector->erase(fileSrc->fileIter);
						} else {
							if (i+1 >= files.size()) {
								destIt = files.begin()+i; //Don't insert out of bounds if the file is moved within the same vector
							}
							if (fileSrc->fileIter < destIt) {
								std::rotate(fileSrc->fileIter, fileSrc->fileIter+1, destIt+1);
							} else if (fileSrc->fileIter > destIt) {
								std::rotate(destIt, fileSrc->fileIter, fileSrc->fileIter+1);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			enableFileContextPopup(window); //Show generic file popup when right clicked
		}
	}
	ImGui::EndChild();
}
