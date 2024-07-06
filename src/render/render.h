#pragma once

#include "shader.h"
#include "utils/enum.h"
#include "viewer/scene.h"
#include <GLFW/glfw3.h>
#include <memory>

class Render {
public:
    virtual ~Render() = default;

    // Initialize when the renderer is created
    virtual void init() {}

    // Setup resources (VAOs, VBOs and textures) for all models in the input scene
    virtual void setup(const std::shared_ptr<Scene>& scene) {}
    // Setup and clean resources (VAOs, VBOs and textures) bind with one input model
    // When add/remove model one by one, call `setupModel/cleanModel` is faster than `setup`
    virtual void setupModel(const ModelPtr& model) {}
    virtual void cleanModel(const ModelPtr& model) {}

    // Render model for the input VP matrix (called every frame in viewer's loop)
    virtual void render(
        const std::shared_ptr<Scene>& scene, 
        const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix
    ) {}

    // Render model indices (for object selection)
    virtual void renderIdx(
        const std::shared_ptr<Scene>& scene, 
        const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix
    ) {}

    // Cleanup when the renderer is destroyed
    virtual void cleanup() {}

    [[nodiscard]] virtual RENDERER_TYPE getType() const { return RENDERER_TYPE::None; }

    [[nodiscard]] const std::unordered_map<SHADER_TYPE, std::shared_ptr<ShaderProgram>>& getShaders() const { return mShaders; }
    [[nodiscard]] const std::pair<SHADER_TYPE, std::shared_ptr<ShaderProgram>>& getCurrentShader() const { return mCurrentShader; }
    void setCurrentShader(SHADER_TYPE type) {
        auto it = mShaders.find(type);
        if (it != mShaders.end()) {
            mCurrentShader = *it;  mCurrentShader.second->use();
        }
    }

protected:
    std::unordered_map<SHADER_TYPE, std::shared_ptr<ShaderProgram>> mShaders;
    std::pair<SHADER_TYPE, std::shared_ptr<ShaderProgram>> mCurrentShader;
};
