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

        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(30, 610), ImGuiCond_Once);

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
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Reset").x);
        PushStyleRedButton();
        if (ImGui::Button("Reset")) {
            viewer.getCamera()->resetControl();
        }
        ImGui::PopStyleColor(3);
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

        // Postion and Orientation
        ImGui::Spacing();
        ImGui::TextWrapped("Position & Orientation");

        glm::vec3 position = viewer.getCamera()->getPosition();
        float yaw = viewer.getCamera()->getYaw();
        float pitch = viewer.getCamera()->getPitch();
        float _pos[3] = { position.x, position.y, position.z };

        float colWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x * 2) / 4.0f;
        ImGui::PushItemWidth(colWidth);

        // X and Yaw row
        ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize("X").x);
        ImGui::Text("X");
        ImGui::SameLine();
        if (ImGui::InputFloat("##X", &_pos[0], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {  // When left focus
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + colWidth - ImGui::CalcTextSize("Yaw").x);
        ImGui::Text("Yaw");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Yaw", &yaw, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            viewer.getCamera()->setYaw(yaw);
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            viewer.getCamera()->setYaw(yaw);
        }

        // Y and Pitch row
        ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize("Y").x);
        ImGui::Text("Y");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Y", &_pos[1], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + colWidth - ImGui::CalcTextSize("Pitch").x);
        ImGui::Text("Pitch");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Pitch", &pitch, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            viewer.getCamera()->setPitch(pitch);
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            viewer.getCamera()->setPitch(pitch);
        }

        // Z row
        ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize("Z").x);
        ImGui::Text("Z");
        ImGui::SameLine();
        if (ImGui::InputFloat("##Z", &_pos[2], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            viewer.getCamera()->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
        }
        ImGui::PopItemWidth();
        
        ImGui::End();
    }
};
