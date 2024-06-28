#pragma once

#include "utils/enum.h"
#include "viewer/scene.h"
#include <GLFW/glfw3.h>
#include <memory>

class Render {
public:
    virtual ~Render() = default;

    // Initialize when the renderer is created
    virtual void init() = 0;

    // Setup buffers and textures for the input scene
    virtual void setup(const std::shared_ptr<Scene>& scene) = 0;

    // Render for the input VP matrix (called every frame in viewer's loop)
    virtual void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

    // Cleanup when the renderer is destroyed
    virtual void cleanup() = 0;

    virtual RENDERER_TYPE getType() const = 0;
};
