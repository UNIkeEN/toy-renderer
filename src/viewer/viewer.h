#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "camera.h"
#include "scene.h"
#include "render/render.h"
#include "widgets/widget.h"

class Viewer {
public:
    Viewer(int width, int height, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene);
    virtual ~Viewer();

    void init() { throw std::runtime_error("API deprecated"); } // Deprecated, use initWindow() and initBackend() instead
    void initWindow();
    void initBackend(); 
    // Need to call by this order: 
    // 1. create and set viewer->mRender,
    // 2. viewer->initWindow(), init GLFW, create window and set opengl context, specially pass mWindow to mRender (if use vulkan)
    // 3. render->init(), init render itself
    // 4. viewer->initBackend(), init ImGUI implementation for different render backend
    // 5. further steps, such as setup scene, etc.

    void mainLoop();
    void cleanup();

    void switchBackend(RENDERER_TYPE type);
    // cleanup and reinit, auto do the steps above.

    [[nodiscard]] GLFWwindow* getWindow() const { return mWindow; }
    [[nodiscard]] std::shared_ptr<Render> getRender() const { return mRender; }
    [[nodiscard]] std::shared_ptr<Camera> getCamera() const { return mCamera; }
    void setCamera(const std::shared_ptr<Camera>& camera) { mCamera = camera; mCamera->update(); }
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
    int mPressedMouseButton; // Which mouse button is pressed (-1 as none, using GLFW enum)
    double mLastX;       // Last mouse x position
    double mLastY;       // Last mouse y position
    double mDeltaTime;   // Interval between current frame and last frame
    double mLastFrame;

    float mMovementSpeed;   // Camera movement speed of keyboard input
    float mMouseSensitivity;

    // User input handling functions and callbacks
    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);    // Callback when user resize the window
    void processGamepadInput();
    
    // Utility functions
    void saveScreenshot();

    std::vector<std::shared_ptr<Widget>> mWidgets;  // ImGUI widgets
    // ImGUI rendering functions
    void renderMainMenu();
    void renderWidgets();
    void createNotification(const std::string& msg, int duration);     // Add new notification widget
};
