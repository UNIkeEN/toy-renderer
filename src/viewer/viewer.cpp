#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "viewer/viewer.h"
#include "render/render_OpenGL.h"
#include "render/render_Vulkan.h"
#include "utils/file.h"
#include "widgets/widget_notification.hpp"

Viewer::Viewer(int width, int height, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene)
    : mWidth(width), mHeight(height), mWindow(nullptr), mCamera(std::move(camera)), mScene(std::move(scene)), mRender(std::make_shared<Render>()),
      mFirstMouse(true), mPressedMouseButton(-1), mLastX(static_cast<float>(width) / 2.0f), mLastY(static_cast<float>(height) / 2.0f), mDeltaTime(0.0f), mLastFrame(0.0f),
      mMovementSpeed(20.0f), mMouseSensitivity(0.15f), 
      mWidgets(createAllWidgets()) {}

Viewer::~Viewer() {
    cleanup();
}

void Viewer::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Create GLFW window
    if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Disable OpenGL context for Vulkan
    } else if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    mWindow = glfwCreateWindow(mWidth, mHeight, "Toy Renderer", nullptr, nullptr);
    if (!mWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(mWindow, this);

    // Callback functions to process user's input
    glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_cast<Viewer*>(glfwGetWindowUserPointer(window))->keyboardCallback(window, key, scancode, action, mods);
    });

    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos) {
        static_cast<Viewer*>(glfwGetWindowUserPointer(window))->mouseCallback(window, xpos, ypos);
    });

    glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
        static_cast<Viewer*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset);
    });

    glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
        static_cast<Viewer*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(window, button, action, mods);
    });

    glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
        static_cast<Viewer*>(glfwGetWindowUserPointer(window))->framebufferSizeCallback(window, width, height);
    });

    // Catch cursor
    // glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set OpenGL context
    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        glfwMakeContextCurrent(mWindow); 
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        std::dynamic_pointer_cast<VulkanRender>(mRender)->setWindow(mWindow);
        glfwMakeContextCurrent(nullptr); 
    }
}

void Viewer::initBackend() {
    // Initialize ImGui context, custom style, and GLFW/OpenGL/Vulkan bindings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Font
    io.Fonts->AddFontFromFileTTF(findFile("assets/fonts/NotoSansMono-Regular.ttf").c_str(), 18.0f);
    // DPI Scaling by monitor resolution
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    io.FontGlobalScale = 1.25f;
    if (mode->width >= 2560 && mode->height >= 1440) { // >=2K
        io.FontGlobalScale = 1.5f;
    } else if (mode->width <= 1280 && mode->height <= 720) { // <=720p
        io.FontGlobalScale = 1.0f;
    }
    // Color Style
    ImGui::StyleColorsDark();

    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init("#version 450");
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        ImGui_ImplGlfw_InitForVulkan(mWindow, true);
        ImGui_ImplVulkan_Init(
            std::dynamic_pointer_cast<VulkanRender>(mRender)->getImGuiInitInfo()
        );
    }

    // Disable V-Sync
    // glfwSwapInterval(0);
}


void Viewer::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        double currentFrame = glfwGetTime();
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        glfwPollEvents();
        processGamepadInput();

        RENDERER_TYPE backendType = mRender->getType();
        if (backendType == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_NewFrame();
        } else if (backendType == RENDERER_TYPE::Vulkan) {
            ImGui_ImplVulkan_NewFrame();
        }
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        renderMainMenu();
        renderWidgets();
        if (mRender->getType() == backendType) {
            ImGui::Render();     // only call if backend isn't switched
        }
        
        // Render scene
        if (mRender) {
            mRender->render(
                mScene,
                mCamera->getViewMatrix(),
                mCamera->getProjectionMatrix()
            );
        }

        // Render ImGui
        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            // ImGui_ImplVulkan_RenderDrawData(
            //     ImGui::GetDrawData(),
            //     std::dynamic_pointer_cast<VulkanRender>(mRender)->getCurrentCommandBuffer()
            // );

            // Already call ImGui_ImplVulkan_RenderDrawData in VulkanRender.render()
        }

        // Swap buffers
        glfwSwapBuffers(mWindow);
    }
}

void Viewer::renderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            for (auto& widget : mWidgets) 
                if (widget->getName().find("##") != 0) {
                    bool visible = widget->isVisible();
                    if (ImGui::MenuItem(widget->getName().c_str(), nullptr, &visible)) {
                        widget->toggle();
                    }
                }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Viewer::renderWidgets() {
    for (auto& widget : mWidgets) {
        widget->render(*this);
    }
}

void Viewer::cleanup() {
    if (mWindow) {
        // Cleanup ImGui
        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_Shutdown();
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            ImGui_ImplVulkan_Shutdown();
        }
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Cleanup GLFW
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
        glfwTerminate();
    }
}

void Viewer::switchBackend(RENDERER_TYPE type) {
    if (mRender->getType() == type) return;

    bool winsowStateSaved = false;
    int windowPosX, windowPosY, windowWidth, windowHeight;
    std::string iniSettings;
    if (mWindow) {  // Save window and ImGui state
        winsowStateSaved = true;
        glfwGetWindowPos(mWindow, &windowPosX, &windowPosY);
        glfwGetWindowSize(mWindow, &windowWidth, &windowHeight);
    }
    if (ImGui::GetCurrentContext()) {
        iniSettings = ImGui::SaveIniSettingsToMemory();
    }

    // Don't need cleanup old render explicitly, it will be cleaned by calling its destructor when reset mRender (old render will be destroyed).
    cleanup();

    if (type == RENDERER_TYPE::OpenGL) {
        mRender = std::make_shared<OpenGLRender>();
    } else if (type == RENDERER_TYPE::Vulkan) {
        mRender = std::make_shared<VulkanRender>();
    }

    initWindow();

    if (winsowStateSaved) {
        glfwSetWindowPos(mWindow, windowPosX, windowPosY);
        glfwSetWindowSize(mWindow, windowWidth, windowHeight);
    }
    
    mRender->init();
    initBackend();

    if (!iniSettings.empty()) {
        ImGui::LoadIniSettingsFromMemory(iniSettings.c_str(), iniSettings.size());
    }   // Restore ImGui state
    mRender->setup(mScene);

    // seems must add it
    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        ImGui_ImplOpenGL3_NewFrame();
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        ImGui_ImplVulkan_NewFrame();
    }
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();
}

void Viewer::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        else if (key == GLFW_KEY_F12) saveScreenshot();
        else if (key == GLFW_KEY_DELETE) {
            for (auto& model : mScene->getModels()) {
                if (model->isSelected()) {
                    mRender->cleanModel(model);
                    mScene->removeModel(model);
                }
            }
        }
        else {
            glm::vec3 movement(0.0f), direction(mCamera->getDirection());
            if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
                movement += direction;
            if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
                movement -= direction;
            if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
                movement -= glm::normalize(glm::cross(direction, mCamera->getUp()));
            if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
                movement += glm::normalize(glm::cross(direction, mCamera->getUp()));
            if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
                movement += mCamera->getUp(); // UP
            if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                movement -= mCamera->getUp();
            if (glm::length(movement) > 0.0f) {
                mCamera->move(movement, static_cast<float>(mMovementSpeed * std::max(mDeltaTime, 1.0 / 60.0f)));
            }
        }
    }
}

void Viewer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mPressedMouseButton < 0 || ImGui::GetIO().WantCaptureMouse) return;

    if (mPressedMouseButton == GLFW_MOUSE_BUTTON_MIDDLE) {
        // Click middle button and drag: rotate camera
        if (mFirstMouse) {
            mLastX = xpos;
            mLastY = ypos;
            mFirstMouse = false;
        }
        auto xoffset = static_cast<float>((xpos - mLastX) * mMouseSensitivity);
        auto yoffset = static_cast<float>((mLastY - ypos) * mMouseSensitivity); // Reversed Y
        mLastX = xpos;
        mLastY = ypos;

        mCamera->rotate(xoffset, yoffset);
    } 
}

void Viewer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    mCamera->zoom(static_cast<float>(yoffset));   // Zoom in/out by changing FOV
}

void Viewer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (action == GLFW_PRESS) {
        mPressedMouseButton = button;
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // Click left button: select model
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            mRender->renderIdx(
                mScene,
                mCamera->getViewMatrix(),
                mCamera->getProjectionMatrix()
            );
            unsigned char pixel[4];
            if (mRender->getType() == RENDERER_TYPE::OpenGL) {
                glReadPixels(static_cast<int>(xpos), static_cast<int>(mHeight - ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
            } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
                // TODO
            }
            int modelIdx = pixel[0] + (pixel[1] << 8) + (pixel[2] << 16) - 1;
            if (modelIdx < mScene->getModels().size() && modelIdx >= 0) 
                mScene->toggleSelectModel(mScene->getModels()[modelIdx]);
            else mScene->selectModel(nullptr);
        } 
    } else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            mFirstMouse = true;
        }
        mPressedMouseButton = -1;
    }
}

void Viewer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // TODO: support Vulkan
    if (width == 0 || height == 0) return;
    glViewport(0, 0, width, height);
    mHeight = height;
    mWidth = width;
    mCamera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}

void Viewer::processGamepadInput() {
    if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return;

    GLFWgamepadstate state;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
        // Left thumbstick for camera movement
        glm::vec3 movement(0.0f), direction(mCamera->getDirection());
        if (fabs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > 0.1f) {  // By GPT, design for drift??
            movement -= direction * state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        }
        if (fabs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > 0.1f) {
            movement += glm::normalize(glm::cross(direction, mCamera->getUp())) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        }
        if (glm::length(movement) > 0.0f) {
            mCamera->move(movement, static_cast<float>(mMovementSpeed * std::max(mDeltaTime, 1.0 / 60.0f)));
        }

        // Right thumbstick for camera rotation, using 3x mouse sensitivity
        if (fabs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) > 0.1f || fabs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) > 0.1f) {
            mCamera->rotate(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * mMouseSensitivity * 3, -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * mMouseSensitivity * 3);
        }
    }
}

void Viewer::saveScreenshot() {
    std::filesystem::path shotDir = "screenshots";
    if (!std::filesystem::exists(shotDir)) {
        std::filesystem::create_directory(shotDir);
    }

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::to_string(now_time_t) + ".jpg";     // I've try to format, but Windows has sth wrong
    std::filesystem::path filename = shotDir / timestamp;  

    std::vector<unsigned char> pixels(3 * mWidth * mHeight);
    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        glReadPixels(0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        // TODO
    }
    
    // Flip the image vertically
    std::vector<unsigned char> flippedPixels(3 * mWidth * mHeight);
    for (int y = 0; y < mHeight; ++y) {
        memcpy(&flippedPixels[3 * mWidth * (mHeight - 1 - y)], &pixels[3 * mWidth * y], 3 * mWidth);
    }

    stbi_write_png(filename.string().c_str(), mWidth, mHeight, 3, flippedPixels.data(), 3 * mWidth);
    createNotification("Screenshot saved to " + filename.string(), 3.0f);
}

void Viewer::createNotification(const std::string& msg, int duration) {
    mWidgets.erase(std::remove_if(mWidgets.begin(), mWidgets.end(),
        [](const std::shared_ptr<Widget>& widget) { return widget->getName() == "##Notification"; }),
        mWidgets.end());
    mWidgets.push_back(std::make_shared<NotificationWidget>("##Notification", msg, duration));
}