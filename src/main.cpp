#include <glad/glad.h>
#include "viewer/viewer.h"
#include "render/OpenGLRender.hpp"

int main() {

    std::shared_ptr<Render> _render = std::make_shared<OpenGLRender>();

    Viewer viewer(800, 600, _render, nullptr);
    viewer.init();
    viewer.mainLoop();
    viewer.cleanup();
    return 0;
}