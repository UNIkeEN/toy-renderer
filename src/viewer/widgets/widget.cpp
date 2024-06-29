#include "widget.h"
#include "widget_HUD.hpp"
#include "widget_model_panel.hpp"
#include "widget_camera_panel.hpp"
#include "widget_top_toolbar.hpp"

std::vector<std::shared_ptr<Widget>> createAllWidgets() {
    std::vector<std::shared_ptr<Widget>> widgets;
    widgets.push_back(std::make_shared<HUDWidget>("HUD"));
    widgets.push_back(std::make_shared<ModelPanelWidget>("Models"));
    widgets.push_back(std::make_shared<CameraPanelWidget>("Camera Control"));
    widgets.push_back(std::make_shared<TopToolbarWidget>("##Top Toolbar"));
    return widgets;
}

inline void PushStyleRedButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.4f, 0.4f, 1.0f));
}