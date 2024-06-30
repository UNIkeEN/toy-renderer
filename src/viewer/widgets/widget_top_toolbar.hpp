#pragma once

#include "widget.h"
#include "../viewer.h"
#include <iostream>

class TopToolbarWidget : public Widget {
public:
    explicit TopToolbarWidget(std::string name) : Widget(std::move(name)) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        auto renderer = viewer.getRender();
        if (!renderer) return;

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 150, ImGui::GetFrameHeight()), ImGuiCond_Always);
        ImGui::Begin(mName.c_str(), &mVisible, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip);

        std::vector<std::tuple<const char*, const char*, SHADER_TYPE>> shaderButtons = {
            {"W", "Wireframe", SHADER_TYPE::Wireframe},
            {"S", "Solid", SHADER_TYPE::Solid},
            {"M", "Material Preview", SHADER_TYPE::MaterialPreview},
            {"R", "Rendered", SHADER_TYPE::Rendered},
            {"C", "Custom", SHADER_TYPE::Custom}
        };

        for (const auto& shaderButton : shaderButtons) {
            bool shaderExists = renderer->getShaders().find(std::get<2>(shaderButton)) != renderer->getShaders().end();

            if (shaderExists) {
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