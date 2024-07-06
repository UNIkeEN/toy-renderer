#include <memory>
#include "render/render_OpenGL.h"
#include "render/render_Vulkan.h"
#include "viewer/viewer.h"
#include "viewer/camera_perspective.hpp"

int main() {

    const int WIDTH = 2560, HEIGHT = 1440;

    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(WIDTH/(float)HEIGHT);
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    Viewer viewer(WIDTH, HEIGHT, camera, scene);
    // viewer.getScene()->addModel("../assets/SJTU_east_gate_MC/East_Gate_Voxel.obj");
    
    viewer.switchBackend(RENDERER_TYPE::OpenGL);

    viewer.mainLoop();

    viewer.cleanup();
    scene->cleanup();

    // system("pause");
    return 0;
}