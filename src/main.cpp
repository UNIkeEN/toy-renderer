#include <glad/glad.h>
#include <memory>
#include "viewer/viewer.h"
#include "viewer/camera_perspective.hpp"
#include "render/render_OpenGL.h"

int main() {

    const int WIDTH = 1920, HEIGHT = 1080;

    std::shared_ptr<Render> renderer = std::make_shared<OpenGLRender>();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(WIDTH/(float)HEIGHT);
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    Viewer viewer(WIDTH, HEIGHT, renderer, camera, scene);
    // viewer.getScene()->addModel("../assets/SJTU_east_gate_MC/East_Gate_Voxel.obj");

    viewer.init();
    renderer->init();
    renderer->setup(viewer.getScene());

    viewer.mainLoop();
    
    renderer->cleanup();
    viewer.cleanup();
    scene->cleanup();

    // system("pause");
    return 0;
}