#pragma once

#include "widget.h"
// #include "../viewer.h"
#include <string>
#include <chrono>
#include <utility>

class NotificationWidget : public Widget {
public:
    NotificationWidget(const std::string& name, std::string message, float duration)
        : Widget(name), mMessage(std::move(message)), mDuration(duration) {
            mStartTime = std::chrono::system_clock::now();
            show();
        }

    void render(Viewer& viewer) override {
        if (!isVisible()) return;

        auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = now - mStartTime;
        if (elapsed.count() > mDuration) {
            hide();
            return;
        }

        float alpha = 1.0f;
        if (elapsed.count() > mDuration - 1.0f) { // Last second for fading out
            alpha = (mDuration - elapsed.count());
        }
        ImGui::SetNextWindowBgAlpha(alpha);

        ImVec2 textSize = ImGui::CalcTextSize(mMessage.c_str(), nullptr, true, 360.0f);
        ImGui::SetNextWindowSize(ImVec2(360, textSize.y + 80), ImGuiCond_Once);
        ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - 360) / 2, 40);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        
        ImGui::Begin("Notification", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing);
        ImVec4 currentTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        currentTextColor.w = alpha;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(currentTextColor));
        ImGui::TextWrapped("%s", mMessage.c_str());
        ImGui::PopStyleColor();
        ImGui::End();
    }

private:
    std::string mMessage;
    float mDuration;
    std::chrono::time_point<std::chrono::system_clock> mStartTime;
};
