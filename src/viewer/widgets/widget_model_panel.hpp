#pragma once

#include "widget.h"
#include "../viewer.h"
#include <cstdio>
#include <nfd.h>  

class ModelPanelWidget : public Widget {
public:
    explicit ModelPanelWidget(std::string name) : Widget(std::move(name)) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        ImGui::SetNextWindowSize(ImVec2(360, 500), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 100), ImGuiCond_Once);
        
        ImGui::Begin(mName.c_str(), &mVisible);
        
        if (ImGui::Button("Add")) {
            addModel(viewer);
        }

        ImGui::Separator();

        ImGui::BeginChild("ModelList", ImVec2(0, 0), false);
        for (size_t i = 0; i < viewer.getScene()->getModelCount(); ++i) {
            ImGui::PushID(i);
            bool allShapesInvisible = true;
            for (size_t j = 0; j < viewer.getScene()->getShapeCount(i); ++j) {
                if (viewer.getScene()->isShapeVisible(i, j)) {
                    allShapesInvisible = false;
                    break;
                }
            }
            if (allShapesInvisible) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            }
            bool treeOpen = ImGui::TreeNodeEx(viewer.getScene()->getModelName(i).c_str());
            // If use "if (ImGui::TreeNode(...) {sameline, button ...}" then the button will be hidden when the tree is closed.
            if (allShapesInvisible) {
                ImGui::PopStyleColor();
            }

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (treeOpen ? 0 : 20));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.6f, 0.6f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.45f, 0.45f, 1.0f));
            if (ImGui::Button("-")) {
                viewer.getScene()->removeModel(i);
                viewer.getRender()->setup(viewer.getScene());
            }
            ImGui::PopStyleColor(3);

            if (treeOpen) {
                for (size_t j = 0; j < viewer.getScene()->getShapeCount(i); ++j) {
                    ImGui::PushID(j);
                    bool isVisible = viewer.getScene()->isShapeVisible(i, j);
                    if (!isVisible) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    }
                    ImGui::TextWrapped(viewer.getScene()->getShapeName(i, j).c_str());
                    if (!isVisible) {
                        ImGui::PopStyleColor();
                    }

                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
                    if (ImGui::Checkbox("##visible", &isVisible)) {     // Can't make this checkbox between PushStyleColor() and PopStyleColor()!
                        viewer.getScene()->setShapeVisible(i, j, isVisible);
                        viewer.getRender()->setup(viewer.getScene());
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
    static void addModel(Viewer& viewer) {
        // https://github.com/btzy/nativefiledialog-extended?tab=readme-ov-file#basic-usage

        NFD_Init();
        nfdu8char_t *outPath;
        nfdu8filteritem_t filters[1] = {{ "Wavefront", "obj" }};
        nfdopendialogu8args_t args = {nullptr};
        args.filterList = filters;
        args.filterCount = 1;
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
