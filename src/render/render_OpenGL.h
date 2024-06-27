#pragma once

#include "render.h"
#include "shader.h"
#include "viewer/scene.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

class OpenGLRender : public Render {
public:
    OpenGLRender();
    ~OpenGLRender() override;

    void init() override;
    void setupBuffers(const std::shared_ptr<Scene>& scene);
    void render() override;
    void cleanup() override;

    RENDERER_TYPE getType() const override;
    
private:
    std::unique_ptr<ShaderProgram> mShader;
    std::vector<GLuint> mVAOs;
    std::vector<GLuint> mVBOs;
    std::vector<GLuint> mEBOs;
};
