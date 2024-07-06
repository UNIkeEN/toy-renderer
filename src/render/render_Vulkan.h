#pragma once

#include "render.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui_impl_vulkan.h>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct VulkanModelResources {
    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceMemory> vertexBufferMemory;
    std::vector<uint32_t> indicesCount;
    std::vector<VkImage> textures;
    std::vector<VkDeviceMemory> textureMemory;
    std::vector<VkImageView> textureImageViews;
    std::vector<VkSampler> textureSamplers;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    alignas(4) bool hasTexture;
};

const int MAX_FRAMES_IN_FLIGHT = 2;

class VulkanRender : public Render {
public:
    VulkanRender() = default;
    ~VulkanRender() override;
    
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

    [[nodiscard]] RENDERER_TYPE getType() const override;

    void setWindow(GLFWwindow* window) { mWindow = window; }
    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const { return mCommandBuffers[mCurrentFrame]; }
    [[nodiscard]] ImGui_ImplVulkan_InitInfo* getImGuiInitInfo();

private:
    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createUniformBuffers();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createImGuiDescriptorPool();

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool hasTexture);

    void recreateSwapChain();
    void cleanupSwapChain();
    void resetModelResources();

    std::unordered_map<ModelPtr, VulkanModelResources> mModelResources;

    GLFWwindow* mWindow;
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice mDevice;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
    std::vector<VkImageView> mSwapChainImageViews;
    VkRenderPass mRenderPass;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VkDeviceMemory> mUniformBuffersMemory;
    VkDescriptorSetLayout mDescriptorSetLayout;
    VkDescriptorPool mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;
    VkDescriptorPool mImGuiDescriptorPool;
    // Pipeline and pipeline layout is managed by ShaderProgram
    VkPipeline mCurrentPipeline;
    VkPipelineLayout mCurrentPipelineLayout;

    size_t mCurrentFrame = 0;
    bool mFramebufferResized = false;

    const std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
    #ifdef _WIN32
        "VK_KHR_win32_surface" // Can not import vulkan_win32.h, don't know why.
    #elif defined(__linux__)
        "VK_KHR_xcb_surface"
    #endif
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // For createInstance()
    static std::vector<const char*> getRequiredExtensions();
    // For pickPhysicalDevice()
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    // For createLogicalDevice() and createCommandPool()
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    // For createSwapChain()
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void createTextureImage(const std::string& texturePath, VkImage& textureImage, VkDeviceMemory& textureImageMemory);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createTextureImageView(VkImage image, VkImageView& imageView);
    void createTextureSampler(VkSampler& sampler);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
};

