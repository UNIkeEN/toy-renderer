#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <imgui.h>

// Forward declaration to avoid circular dependency
class Viewer;
class HUDWidget;
class ModelPanelWidget;
class CameraPanelWidget;
class TopToolbarWidget;

class Widget {
public:
    explicit Widget(std::string name) : mName(std::move(name)), mVisible(true) {}
    virtual ~Widget() = default;

    virtual void render(Viewer& viewer) = 0;

    void show() { mVisible = true; }
    void hide() { mVisible = false; }
    void toggle() { mVisible = !mVisible; }
    [[nodiscard]] bool isVisible() const { return mVisible; }

    [[nodiscard]] const std::string& getName() const { return mName; }

protected:
    std::string mName;
    bool mVisible;
};

std::vector<std::shared_ptr<Widget>> createAllWidgets();

inline void PushStyleRedButton();