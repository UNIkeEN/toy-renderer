#pragma once

#include "utils/enum.h"
#include "viewer/scene.h"
#include <memory>

class Render {
public:
    virtual ~Render() = default;

    // Initialize when the renderer is created
    virtual void init() = 0;

    // Setup buffers and shaders for the input scene
    virtual void setup(const std::shared_ptr<Scene>& scene) = 0;

    // Render (called every frame in viewer's loop)
    virtual void render() = 0;

    // Cleanup when the renderer is destroyed
    virtual void cleanup() = 0;

    virtual RENDERER_TYPE getType() const = 0;
};
