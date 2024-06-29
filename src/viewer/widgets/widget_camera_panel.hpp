#pragma once

#include "widget.h"
#include "../viewer.h"
#include "../camera.h"
#include "../camera_perspective.hpp"
#include "../camera_orthographic.hpp"

class CameraPanelWidget : public Widget {
public:
    CameraPanelWidget(const std::string& name) : Widget(name) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 410), ImGuiCond_Once);

        ImGui::Begin(mName.c_str(), &mVisible);

        ImGui::TextWrapped("Type");
        const char* cameraTypes[] = { "Perspective", "Orthographic" };
        static int currentCameraType = viewer.getCamera()->getType() == CAMERA_TYPE::Perspective ? 0 : 1;
        if (ImGui::Combo("##Camera Type", &currentCameraType, cameraTypes, IM_ARRAYSIZE(cameraTypes))) {
            if (currentCameraType == 0) {
                viewer.setCamera(std::make_shared<PerspectiveCamera>(*(viewer.getCamera())));
            } else if (currentCameraType == 1) {
                viewer.setCamera(std::make_shared<OrthographicCamera>(*(viewer.getCamera())));
            }
        }
        ImGui::Spacing();

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

        if (viewer.getCamera()->getType() == CAMERA_TYPE::Perspective) {
            ImGui::TextWrapped("FOV");
            float fov = viewer.getCamera()->getFOV();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::SliderFloat("##Camera FOV", &fov, 30.0f, 90.0f)) {
                viewer.getCamera()->setFOV(fov);
            }
            ImGui::PopItemWidth();
        }

        ImGui::End();
    }
};
