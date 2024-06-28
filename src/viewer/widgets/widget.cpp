#include "widget.h"
#include "widget_HUD.hpp"

std::vector<std::shared_ptr<Widget>> createAllWidgets() {
    std::vector<std::shared_ptr<Widget>> widgets;
    widgets.push_back(std::make_shared<HUDWidget>("HUD"));
    return widgets;
}