#include <glad/glad.h>
#include "viewer/viewer.h"
#include "render/render_OpenGL.h"

int main() {

    std::shared_ptr<Render> renderer = std::make_shared<OpenGLRender>();

    Viewer viewer(800, 600, renderer, nullptr);
    viewer.init();
    viewer.mainLoop();
    viewer.cleanup();
    return 0;
}