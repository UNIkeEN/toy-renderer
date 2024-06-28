#include "widget.h"
#include "widget_HUD.hpp"
#include "widget_model_panel.hpp"
#include "widget_camera_panel.hpp"

std::vector<std::shared_ptr<Widget>> createAllWidgets() {
    std::vector<std::shared_ptr<Widget>> widgets;
    widgets.push_back(std::make_shared<HUDWidget>("HUD"));
    widgets.push_back(std::make_shared<ModelPanelWidget>("Model Panel"));
    widgets.push_back(std::make_shared<CameraPanelWidget>("Camera Panel"));
    return widgets;
}