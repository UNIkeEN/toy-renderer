#pragma once

#include <glad/glad.h>
#include "render.h"
#include "shader.h"
#include <vector>

class OpenGLRender : public Render {
public:
    OpenGLRender();
    ~OpenGLRender() override;

    void init() override;
    void setup(const std::shared_ptr<Scene>& scene);
    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
    void cleanup() override;

    RENDERER_TYPE getType() const override;
    
private:
    void loadTexture(const std::string& path, GLuint& textureID);

    std::unique_ptr<ShaderProgram> mShader;
    std::vector<GLuint> mVAOs;
    std::vector<GLuint> mVBOs;
    std::vector<GLuint> mEBOs;
    std::vector<GLuint> mTextures;
    std::vector<size_t> mVertexCounts; // Number of vertices for each model, used in glDrawArrays
};
