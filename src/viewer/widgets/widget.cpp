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