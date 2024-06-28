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

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 100), ImGuiCond_Once);
        
        ImGui::Begin(mName.c_str(), &mVisible);
        
        if (ImGui::Button("Import")) {
            addModel(viewer);
        }

        ImGui::Separator();

        ImGui::BeginChild("ModelList", ImVec2(0, 0), false);
        for (size_t i = 0; i < viewer.getScene()->getModelCount(); i++) {
            ImGui::Text(viewer.getScene()->getModelName(i).c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
            if (ImGui::Button("-")) {
                viewer.getScene()->removeModel(i);
                viewer.getRender()->setup(viewer.getScene());
            }
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
