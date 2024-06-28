#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// #include <imgui_impl_vulkan.h>
#include "camera.h"
#include "scene.h"
#include "render/render.h"
#include "widgets/widget.h"

class Viewer {
public:
    Viewer(int width, int height, std::shared_ptr<Render> render, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene);
    virtual ~Viewer();

    void init();
    void mainLoop();
    void cleanup();

    [[nodiscard]] std::shared_ptr<Render> getRender() const { return mRender; }
    [[nodiscard]] std::shared_ptr<Camera> getCamera() const { return mCamera; }
    void setCamera(const std::shared_ptr<Camera>& camera) { mCamera = camera; }
    [[nodiscard]] std::shared_ptr<Scene> getScene() const { return mScene; }
    void setScene(const std::shared_ptr<Scene>& scene) { mScene = scene; }

    [[nodiscard]] int getWidth() const { return mWidth; }
    [[nodiscard]] int getHeight() const { return mHeight; }
    [[nodiscard]] double getDeltaTime() const { return mDeltaTime; }
    void setMovementSpeed(float speed) { mMovementSpeed = speed; }
    [[nodiscard]] float getMovementSpeed() const { return mMovementSpeed; }
    void setMouseSensitivity(float sensitivity) { mMouseSensitivity = sensitivity; }
    [[nodiscard]] float getMouseSensitivity() const { return mMouseSensitivity; }

protected:
    int mWidth;
    int mHeight;
    GLFWwindow* mWindow;
    std::shared_ptr<Render> mRender;
    std::shared_ptr<Camera> mCamera;
    std::shared_ptr<Scene> mScene;

    bool mFirstMouse;    // If the mouse is first used
    bool mLeftMouseButtonPressed;   
    double mLastX;       // Last mouse x position
    double mLastY;       // Last mouse y position
    double mDeltaTime;   // Interval between current frame and last frame
    double mLastFrame;

    float mMovementSpeed;   // Camera movement speed of keyboard input
    float mMouseSensitivity;

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    std::vector<std::shared_ptr<Widget>> mWidgets;  // ImGUI widgets

    void renderMainMenu();
    void renderWidgets();
};
