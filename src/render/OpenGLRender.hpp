#pragma once

#include "render.h"
#include <GLFW/glfw3.h>
#include <iostream>

class OpenGLRender : public Render {
public:
    OpenGLRender() = default;
    ~OpenGLRender() override = default;

    void init() override {
        // initialize glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    void render() override {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // TODO: render
    }

    void cleanup() override {

        // TODO: cleanup
    }

    RENDERER_TYPE getType() const override {
        return RENDERER_TYPE::OpenGL;
    }
};
