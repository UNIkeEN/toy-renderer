#include "render/render_OpenGL.h"
#include <glad/glad.h>
#include <iostream>

OpenGLRender::OpenGLRender() {}

OpenGLRender::~OpenGLRender() {
    cleanup();
    if (mShader) {
        mShader->cleanup();
    }
}

RENDERER_TYPE OpenGLRender::getType() const {
    return RENDERER_TYPE::OpenGL;
}

void OpenGLRender::init() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Failed to initialize GLAD");
    }

    mShader = std::make_unique<ShaderProgram>("../shaders/OpenGL/vertex.glsl", "../shaders/OpenGL/fragment.glsl");
    mShader->use();
}

void OpenGLRender::setupBuffers(const std::shared_ptr<Scene>& scene) {
    cleanup();
    size_t modelCount = scene->getModelCount();
    mVAOs.resize(modelCount);
    mVBOs.resize(modelCount);

    glGenVertexArrays(modelCount, mVAOs.data());
    glGenBuffers(modelCount, mVBOs.data());

    for (size_t i = 0; i < modelCount; ++i) {
        const std::vector<glm::vec3>& vertices = scene->getVertices(i);
        const std::vector<glm::vec3>& normals = scene->getNormals(i);
        const std::vector<glm::vec2>& texCoords = scene->getTexCoords(i);

        std::vector<float> bufferData;
        for (size_t j = 0; j < vertices.size(); ++j) {
            bufferData.push_back(vertices[j].x);
            bufferData.push_back(vertices[j].y);
            bufferData.push_back(vertices[j].z);

            if (!normals.empty()) {
                bufferData.push_back(normals[j].x);
                bufferData.push_back(normals[j].y);
                bufferData.push_back(normals[j].z);
            }

            if (!texCoords.empty()) {
                bufferData.push_back(texCoords[j].x);
                bufferData.push_back(texCoords[j].y);
            }
        }

        glBindVertexArray(mVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, mVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (normals.empty() ? 3 : 8) * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        if (!normals.empty()) {
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        if (!texCoords.empty()) {
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void OpenGLRender::render() {
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShader->use();
    for (size_t i = 0; i < mVAOs.size(); ++i) {
        glBindVertexArray(mVAOs[i]);
        glDrawArrays(GL_TRIANGLES, 0, mVAOs.size());
        glBindVertexArray(0);
    }
}

void OpenGLRender::cleanup() {
    if (!mVAOs.empty()) {
        glDeleteVertexArrays(mVAOs.size(), mVAOs.data());
        mVAOs.clear();
    }
    if (!mVBOs.empty()) {
        glDeleteBuffers(mVBOs.size(), mVBOs.data());
        mVBOs.clear();
    }
    if (!mEBOs.empty()) {
        glDeleteBuffers(mEBOs.size(), mEBOs.data());
        mEBOs.clear();
    }
}
