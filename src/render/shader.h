#pragma once

#include <glad/glad.h>
#include "utils/enum.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan.h>

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
    ~ShaderProgram();

    void init(RENDERER_TYPE backendType);
    void use();
    void cleanup();

    // For OpenGL Backend
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    // For Vulkan Backend
    void bindUniformBuffer(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
    static void setViewportSize(VkCommandBuffer commandBuffer, float width, float height);
    void setVulkanDevice(VkDevice device) { mDevice = device; }
    void setVulkanRenderPass(VkRenderPass renderPass) { mRenderPass = renderPass; }
    VkPipeline getGraphicsPipeline() { return mPipeline; }
    VkPipelineLayout getPipelineLayout() { return mPipelineLayout; }

private:
    std::string mVertexPath;
    std::string mFragmentPath;
    std::string mGeometryPath;

    RENDERER_TYPE mBackendType;

    // For OpenGL Backend
    void initForOpenGL();
    void useForOpenGL();
    void cleanupForOpenGL();
    GLuint mProgram;
    static GLuint compileOpenGLShader(const std::string& path, GLenum type);

    // For Vulkan Backend
    void initForVulkan();
    void useForVulkan();
    void cleanupForVulkan();
    VkDevice mDevice;
    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
    VkRenderPass mRenderPass;
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<uint32_t>& code);
    VkPipeline createGraphicsPipeline();
    static std::vector<uint32_t> compileGLSLToSPV(const std::string& source, EShLanguage shaderType);
};
