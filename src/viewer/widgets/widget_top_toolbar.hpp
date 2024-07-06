#pragma once

#include "widget.h"
#include "../viewer.h"

class TopToolbarWidget : public Widget {
public:
    explicit TopToolbarWidget(std::string name) : Widget(std::move(name)) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        auto renderer = viewer.getRender();
        if (!renderer) return;

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 355, ImGui::GetFrameHeight()), ImGuiCond_Always);
        ImGui::Begin(mName.c_str(), &mVisible, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip);

        const char* backendTypes[] = { "OpenGL", "Vulkan(Beta)", "Unknown" };
        static int currentBackendType = (renderer->getType() == RENDERER_TYPE::OpenGL ? 0 : 
                                        (renderer->getType() == RENDERER_TYPE::Vulkan ? 1 : 2));
        bool switchRequested = false;
        RENDERER_TYPE newType = RENDERER_TYPE::None;
        ImGui::PushItemWidth(200);
        if (ImGui::BeginCombo("##Backend Type", backendTypes[currentBackendType])) {
            ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetTextLineHeightWithSpacing() * 2));
            for (int n = 0; n < IM_ARRAYSIZE(backendTypes) - 1; n++) {
                bool is_selected = (currentBackendType == n);
                if (ImGui::Selectable(backendTypes[n], is_selected)) {
                    if (currentBackendType != n) {
                        switchRequested = true;
                        newType = (n == 0) ? RENDERER_TYPE::OpenGL : RENDERER_TYPE::Vulkan;
                        currentBackendType = n;
                    }
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        if (switchRequested) {
            viewer.switchBackend(newType);
            return;
        }

        ImGui::SameLine();

        static std::vector<std::tuple<const char*, const char*, SHADER_TYPE>> shaderButtons = {
            {"W", "Wireframe", SHADER_TYPE::Wireframe},
            {"S", "Solid", SHADER_TYPE::Solid},
            {"M", "Material Preview", SHADER_TYPE::MaterialPreview},
            {"R", "Rendered", SHADER_TYPE::Rendered},
            {"C", "Custom", SHADER_TYPE::Custom}
        };

        for (const auto& shaderButton : shaderButtons) {
            if (renderer->getShaders().find(std::get<2>(shaderButton)) != renderer->getShaders().end()) {
                bool isSelected = (renderer->getCurrentShader().first == std::get<2>(shaderButton));
                if (isSelected) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                }

                if (ImGui::Button(std::get<0>(shaderButton))) {
                    if (!isSelected) {
                        renderer->setCurrentShader(std::get<2>(shaderButton));
                    }
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Viewport Shading: %s", std::get<1>(shaderButton));
                }

                if (isSelected) {
                    ImGui::PopStyleColor();
                }
            } else {
                ImGui::BeginDisabled();
                ImGui::Button(std::get<0>(shaderButton));
                ImGui::EndDisabled();
            }
            ImGui::SameLine();
        }

        ImGui::End();
    }
};