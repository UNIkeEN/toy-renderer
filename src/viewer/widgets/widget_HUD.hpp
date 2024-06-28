#include <utility>

#include "widget.h"
#include "../viewer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class HUDWidget : public Widget {
public:
    explicit HUDWidget(std::string name) : Widget(std::move(name)) {}

    void render(Viewer& viewer) override {
        if (!mVisible) return;
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin(mName.c_str(), &mVisible, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
        ImGui::TextWrapped("FPS: %.2f", 1.0f / viewer.getDeltaTime());
        ImGui::End();

        drawCoordinateAxes(viewer);
    }

    private:
    void drawCoordinateAxes(Viewer& viewer) {

        // Guided by GPT
        
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        ImVec2 origin = ImVec2(io.DisplaySize.x - 90, io.DisplaySize.y - 90);  // Right-bottom corner
        float axis_length = 45.0f;

        auto camera = viewer.getCamera();
        glm::vec3 forward = glm::normalize(camera->getDirection());
        glm::vec3 right = glm::normalize(glm::cross(forward, camera->getUp()));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        ImVec2 forward2D = ImVec2(forward.x * axis_length, -forward.y * axis_length);
        ImVec2 right2D = ImVec2(right.x * axis_length, -right.y * axis_length);
        ImVec2 up2D = ImVec2(up.x * axis_length, -up.y * axis_length);
        
        // X-axis
        draw_list->AddLine(origin, ImVec2(origin.x + right2D.x, origin.y + right2D.y), IM_COL32(255, 109, 109, 255), 2.0f);
        draw_list->AddText(ImVec2(origin.x + right2D.x + 5, origin.y + right2D.y - 10), IM_COL32(255, 109, 109, 255), "X");

        // Y-axis
        draw_list->AddLine(origin, ImVec2(origin.x + up2D.x, origin.y + up2D.y), IM_COL32(109, 255, 109, 255), 2.0f);
        draw_list->AddText(ImVec2(origin.x + up2D.x - 15, origin.y + up2D.y - 10), IM_COL32(109, 255, 109, 255), "Y");

        // Z-axis
        draw_list->AddLine(origin, ImVec2(origin.x + forward2D.x, origin.y + forward2D.y), IM_COL32(109, 109, 255, 255), 2.0f);
        draw_list->AddText(ImVec2(origin.x + forward2D.x - 20, origin.y + forward2D.y + 10), IM_COL32(109, 109, 255, 255), "Z");
    }
};