#pragma once

#include "widget.h"
#include "../viewer.h"

class ModelPanelWidget : public Widget {
public:
    explicit ModelPanelWidget(const std::string& name) : Widget(name) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;

        ModelPtr selectModel = nullptr;
        for (const auto &model : viewer.getScene()->getModels()){
            if (model->isSelected()){
                selectModel = model;
                break;
            }
        }
        if (selectModel == nullptr) return; // No model selected

        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 430, 100), ImGuiCond_Always);

        ImGui::Begin(mName.c_str(), &mVisible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::TextWrapped("%s", selectModel->getName().c_str());
        size_t visibleShapes = 0;
        for (size_t i = 0; i < selectModel->getShapeCount(); ++i)
            if (selectModel->isShapeVisible(i)) visibleShapes++;

        ImGui::TextWrapped("%d/%d shape%s visible.", visibleShapes, selectModel->getShapeCount(), visibleShapes > 1 ? "s are" : " is");
        ImGui::Separator();
        ImGui::TextWrapped("Transform");
        ImGui::Spacing();

        glm::vec3 position = selectModel->getPosition();
        glm::vec3 rotation = selectModel->getRotation();
        glm::vec3 scale = selectModel->getScale();

        float _pos[3] = { position.x, position.y, position.z };
        float _rot[3] = { rotation.x, rotation.y, rotation.z };
        float _scale[3] = { scale.x, scale.y, scale.z };

        float colWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x * 2) / 2.0f;
        ImGui::PushItemWidth(colWidth);

        // Position controls
        for (int i = 0; i < 3; ++i) {
            static const char* labels[3] = { "Location X", "Y", "Z" };
            ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize(labels[i]).x);
            ImGui::Text("%s", labels[i]);
            ImGui::SameLine();
            std::string id = "##pos" + std::to_string(i);
            if (ImGui::InputFloat(id.c_str(), &_pos[i], 0.01f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                selectModel->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                selectModel->setPosition(glm::vec3(_pos[0], _pos[1], _pos[2]));
            }
        }
        ImGui::Spacing();

        // Rotation controls
        for (int i = 0; i < 3; ++i) {
            static const char* labels[3] = { "Rotation X", "Y", "Z" };
            ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize(labels[i]).x);
            ImGui::Text("%s", labels[i]);
            ImGui::SameLine();
            std::string id = "##rot" + std::to_string(i);
            if (ImGui::InputFloat(id.c_str(), &_rot[i], 1.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                selectModel->setRotation(glm::vec3(_rot[0], _rot[1], _rot[2]));
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                selectModel->setRotation(glm::vec3(_rot[0], _rot[1], _rot[2]));
            }
        }
        ImGui::Spacing();

        // Scale controls
        for (int i = 0; i < 3; ++i) {
            static const char* labels[3] = { "Scale X", "Y", "Z" };
            ImGui::SetCursorPosX(colWidth - ImGui::CalcTextSize(labels[i]).x);
            ImGui::Text("%s", labels[i]);
            ImGui::SameLine();
            std::string id = "##scale" + std::to_string(i);
            if (ImGui::InputFloat(id.c_str(), &_scale[i], 0.01f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                selectModel->setScale(glm::vec3(_scale[0], _scale[1], _scale[2]));
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                selectModel->setScale(glm::vec3(_scale[0], _scale[1], _scale[2]));
            }
        }

        ImGui::PopItemWidth();

        ImGui::End();
    }
};
