#include "shader.h"

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath ) :
    mVertexPath(vertexPath), mFragmentPath(fragmentPath), mGeometryPath(geometryPath),
    mBackendType(RENDERER_TYPE::None), 
    mProgram(0), 
    mPipeline(VK_NULL_HANDLE), mPipelineLayout(VK_NULL_HANDLE), mDevice(VK_NULL_HANDLE), mRenderPass(VK_NULL_HANDLE) {}

ShaderProgram::~ShaderProgram() {
    cleanup();
}

void ShaderProgram::init(RENDERER_TYPE backendType) {
    mBackendType = backendType;
    if (mBackendType == RENDERER_TYPE::OpenGL) {
        initForOpenGL();
    } else if (mBackendType == RENDERER_TYPE::Vulkan) {
        initForVulkan();
    }
}

void ShaderProgram::use() {
    if (mBackendType == RENDERER_TYPE::OpenGL) {
        useForOpenGL();
    } else if (mBackendType == RENDERER_TYPE::Vulkan) {
        useForVulkan();
    }
}

void ShaderProgram::cleanup() {
    if (mBackendType == RENDERER_TYPE::OpenGL) {
        cleanupForOpenGL();
    } else if (mBackendType == RENDERER_TYPE::Vulkan) {
        cleanupForVulkan();
    }
}