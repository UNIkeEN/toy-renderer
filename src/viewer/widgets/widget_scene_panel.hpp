#pragma once

#include "widget.h"
#include "../viewer.h"
#include <cstdio>
#include <iostream>
#include <nfd.h>  

class ScenePanelWidget : public Widget {
public:
    explicit ScenePanelWidget(std::string name) : Widget(std::move(name)) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 100), ImGuiCond_Once);
        
        ImGui::Begin(mName.c_str(), &mVisible);
        
        if (ImGui::Button("Add")) {
            addModel(viewer);
        }

        ImGui::Separator();

        ImGui::BeginChild("ModelList", ImVec2(0, 0), false);

        // if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
        //     ImGui::SetWindowFocus();
        //     viewer.getScene()->selectModel(INT_MAX);  // Deselect all models
        // }
        // It make bugs
        
        for (size_t i = 0; i < viewer.getScene()->getModelCount(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            bool allShapesInvisible = true;
            bool modelSelected = false;
            for (size_t j = 0; j < viewer.getScene()->getShapeCount(i); ++j) {
                if (viewer.getScene()->isShapeVisible(i, j)) {
                    allShapesInvisible = false;
                    break;
                }
            }
            for (size_t j = 0; j < viewer.getScene()->getShapeCount(i); ++j) {
                if (viewer.getScene()->isShapeSelected(i, j)) {
                    modelSelected = true;
                    break;
                }
            }

            if (allShapesInvisible) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            } else if (modelSelected) {
                ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)); 
            }

            std::string id = "##Model Item" + std::to_string(i);
            bool treeOpen = ImGui::TreeNodeEx(id.c_str(), ImGuiTreeNodeFlags_AllowOverlap | (modelSelected == true ? ImGuiTreeNodeFlags_Selected : 0));

            ImGui::SameLine();
            ImGui::TextWrapped("%s", viewer.getScene()->getModelName(i).c_str());
            if (ImGui::IsItemClicked()) {
                viewer.getScene()->toggleSelectModel(i);
            }
            // If use "if (ImGui::TreeNode(...) {sameline, button ...}" then the button will be hidden when the tree is closed.
            if (allShapesInvisible || modelSelected) ImGui::PopStyleColor();

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Remove").x + (treeOpen ? 15.0f : -5.0f));
            PushStyleRedButton();
            if (ImGui::Button("Remove")) {
                viewer.getScene()->removeModel(i);
                viewer.getRender()->setup(viewer.getScene());
            }
            ImGui::PopStyleColor(3);

            if (treeOpen) {
                for (size_t j = 0; j < viewer.getScene()->getShapeCount(i); ++j) {
                    ImGui::PushID(static_cast<int>(j));
                    bool isVisible = viewer.getScene()->isShapeVisible(i, j);
                    if (!isVisible) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    }
                    ImGui::TextWrapped("%s", viewer.getScene()->getShapeName(i, j).c_str());
                    if (!isVisible) {
                        ImGui::PopStyleColor();
                    }
                    if (ImGui::IsItemClicked()) {
                        viewer.getScene()->toggleSelectModel(i);
                        modelSelected = !modelSelected;
                    }

                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
                    if (ImGui::Checkbox("##visible", &isVisible)) {     // Can't make this checkbox between PushStyleColor() and PopStyleColor()!
                        viewer.getScene()->setShapeVisible(i, j, isVisible);
                        // viewer.getRender()->setup(viewer.getScene());
                    }
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::EndChild();

        ImGui::End();
    }

private:
    static void addModel(const Viewer& viewer) {
        // https://github.com/btzy/nativefiledialog-extended?tab=readme-ov-file#basic-usage

        NFD_Init();
        nfdu8char_t *outPath;

        std::vector<nfdu8filteritem_t> filters;
        for (const auto& format : viewer.getScene()->supportedFormats) {
            nfdu8filteritem_t filter;
            filter.name = format.second.c_str();
            filter.spec = format.first.c_str() + 1;  // Skip the dot
            filters.push_back(filter);
        }
        nfdopendialogu8args_t args = {nullptr};
        args.filterList = filters.data();
        args.filterCount = filters.size();
        nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        if (result == NFD_OKAY) {
            viewer.getScene()->addModel(outPath);
            viewer.getRender()->setup(viewer.getScene());
            NFD_FreePathU8(outPath);
        }
        else if (result == NFD_CANCEL) {
            printf("User pressed cancel.\n");
        }
        else {
            printf("Error: %s\n", NFD_GetError());
        }

        NFD_Quit();
    }
};
