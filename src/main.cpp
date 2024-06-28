#include <glad/glad.h>
#include "viewer/viewer.h"
#include "viewer/camera_perspective.hpp"
#include "render/render_OpenGL.h"

int main() {

    const int WIDTH = 1920, HEIGHT = 1080;

    std::shared_ptr<Render> renderer = std::make_shared<OpenGLRender>();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(WIDTH/(float)HEIGHT);
    Viewer viewer(WIDTH, HEIGHT, renderer, camera);
    Scene scene;
    scene.addModel("../assets/SJTU_east_gate_MC/East_Gate_Voxel.obj");

    viewer.init();
    renderer->init();
    renderer->setup(std::make_shared<Scene>(scene));

    viewer.mainLoop();
    
    renderer->cleanup();
    viewer.cleanup();
    scene.cleanup();

    // system("pause");
    return 0;
}