#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include "viewer/viewer.h"
#include "utils/file.h"

Viewer::Viewer(int width, int height, std::shared_ptr<Render> render, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene)
    : mWidth(width), mHeight(height), mWindow(nullptr), mRender(std::move(render)), mCamera(std::move(camera)), mScene(std::move(scene)),
      mFirstMouse(true), mPressedMouseButton(-1), mLastX(static_cast<float>(width) / 2.0f), mLastY(static_cast<float>(height) / 2.0f), mDeltaTime(0.0f), mLastFrame(0.0f),
      mMovementSpeed(20.0f), mMouseSensitivity(0.15f), 
      mWidgets(createAllWidgets()) {}

Viewer::~Viewer() {
    cleanup();
}

void Viewer::init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Create GLFW window
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); // Maximize
    mWindow = glfwCreateWindow(mWidth, mHeight, "Toy Renderer", nullptr, nullptr);
    if (!mWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, this);    // For callback functions below to get (Guided by GPT)

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
        // TODO
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

        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_NewFrame();
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            // ImGui_ImplVulkan_NewFrame();
        }
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        renderMainMenu();
        renderWidgets();

        // Render scene
        if (mRender) {
            mRender->render(
                mScene,
                mCamera->getViewMatrix(),
                mCamera->getProjectionMatrix()
            );
        }

        // Render ImGui
        ImGui::Render();
        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
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
            // ImGui_ImplVulkan_Shutdown();
        }
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Cleanup GLFW
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;

        glfwTerminate();
    }
}

void Viewer::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    if (width == 0 || height == 0) return;
    glViewport(0, 0, width, height);
    mCamera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}