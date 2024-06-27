#include "viewer.h"
#include "camera.h"
#include "render/render.h"
#include <iostream>

Viewer::Viewer(int width, int height, std::shared_ptr<Render> render, std::shared_ptr<Camera> camera)
    : mWidth(width), mHeight(height), mWindow(nullptr), mRender(render), mCamera(camera) {}

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
    mWindow = glfwCreateWindow(mWidth, mHeight, "Toy Renderer", nullptr, nullptr);
    if (!mWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(mWindow);

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init("#version 450");
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        ImGui_ImplGlfw_InitForVulkan(mWindow, true);
        // TODO
    }

    mRender->init();
}

void Viewer::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();

        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_NewFrame();
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            // ImGui_ImplVulkan_NewFrame();
        }
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // TODO: Render UI

        if (mRender) {
            mRender->render();
        }

        // Rendering ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(mWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        if (mRender->getType() == RENDERER_TYPE::OpenGL) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
            // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
        }

        // Swap buffers
        glfwSwapBuffers(mWindow);
    }
}

void Viewer::cleanup() {
    // Cleanup ImGui
    if (mRender->getType() == RENDERER_TYPE::OpenGL) {
        ImGui_ImplOpenGL3_Shutdown();
    } else if (mRender->getType() == RENDERER_TYPE::Vulkan) {
        // ImGui_ImplVulkan_Shutdown();
    }
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup GLFW
    if (mWindow) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }
    glfwTerminate();
}
