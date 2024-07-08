#pragma once

#include <vector>
#include <glad/glad.h>
#include "render.h"

struct OpenGLModelResources {
    std::vector<GLuint> VAOs;
    std::vector<GLuint> VBOs;
    std::vector<GLuint> textures;
    std::vector<size_t> vertexCounts;
};

class OpenGLRender : public Render {
public:
    OpenGLRender() = default;
    ~OpenGLRender() override;

    void init() override;
    void setup(const std::shared_ptr<Scene>& scene) override;
    void setupModel(const ModelPtr& model) override;
    void cleanModel(const ModelPtr& model) override;
    void render(
        const std::shared_ptr<Scene>& scene, 
        const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix
    ) override;
    void renderIdx(
        const std::shared_ptr<Scene>& scene, 
        const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix
    ) override;
    void cleanup() override;
    void viewportResizeCallback(int width, int height) override;

    [[nodiscard]] RENDERER_TYPE getType() const override;
    
private:
    static void loadTexture(const std::string& path, GLuint& textureID);

    std::unordered_map<ModelPtr, OpenGLModelResources> mModelResources;
};
