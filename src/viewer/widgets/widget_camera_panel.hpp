#pragma once

#include "widget.h"
#include "../viewer.h"
#include "../camera.h"

class CameraPanelWidget : public Widget {
public:
    CameraPanelWidget(const std::string& name) : Widget(name) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 410), ImGuiCond_Once);

        ImGui::Begin(mName.c_str(), &mVisible);

        ImGui::TextWrapped("Move Speed");
        float movementSpeed = viewer.getMovementSpeed();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderFloat("##Move Speed", &movementSpeed, 1.0f, 50.0f)) {
            viewer.setMovementSpeed(movementSpeed);
        }
        ImGui::PopItemWidth();
        ImGui::Spacing();

        ImGui::TextWrapped("Mouse Sensitivity");
        float mouseSensitivity = viewer.getMouseSensitivity();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderFloat("##Mouse Sensitivity", &mouseSensitivity, 0.01f, 0.50f)) {
            viewer.setMouseSensitivity(mouseSensitivity);
        }
        ImGui::PopItemWidth();
        ImGui::Spacing();

        if (auto camera = std::dynamic_pointer_cast<Camera>(viewer.getCamera())) {
            ImGui::TextWrapped("FOV");
            float fov = camera->getFOV();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::SliderFloat("##Camera FOV", &fov, 30.0f, 90.0f)) {
                camera->setFOV(fov);
            }
            ImGui::PopItemWidth();
        }

        ImGui::End();
    }
};
