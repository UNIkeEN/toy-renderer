#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// #include <imgui_impl_vulkan.h>
#include "render/render.h"
#include "camera.h"

class Viewer {
public:
    Viewer(int width, int height, std::shared_ptr<Render> render, std::shared_ptr<Camera> camera);
    virtual ~Viewer();

    void init();
    void mainLoop();
    void cleanup();

    std::shared_ptr<Camera> getCamera() const { return mCamera; }
    void setCamera(const std::shared_ptr<Camera>& camera) { mCamera = camera; }

protected:
    int mWidth;
    int mHeight;
    GLFWwindow* mWindow;
    std::shared_ptr<Render> mRender;
    std::shared_ptr<Camera> mCamera;
};
