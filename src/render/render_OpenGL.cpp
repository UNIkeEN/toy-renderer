#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "render_OpenGL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils/file.h"
#include <iostream>

OpenGLRender::~OpenGLRender() {
    OpenGLRender::cleanup();
    for (auto& shader : mShaders) {
        shader.second->cleanup();
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

    mShaders[SHADER_TYPE::Solid] = std::make_shared<ShaderProgram>(
        findFile("assets/shaders/glsl/solid.vert"),
        findFile("assets/shaders/glsl/solid.frag")
    );
    mShaders[SHADER_TYPE::MaterialPreview] = std::make_shared<ShaderProgram>(
        findFile("assets/shaders/glsl/material-preview.vert"),
        findFile("assets/shaders/glsl/material-preview.frag")
    );
    mShaders[SHADER_TYPE::Wireframe] = std::make_shared<ShaderProgram>(
        findFile("assets/shaders/glsl/wireframe.vert"),
        findFile("assets/shaders/glsl/wireframe.frag")
    );
    mShaders[SHADER_TYPE::Outline] = std::make_shared<ShaderProgram>(
        findFile("assets/shaders/glsl/outline.vert"),
        findFile("assets/shaders/glsl/outline.frag")
    );
    mShaders[SHADER_TYPE::Index] = std::make_shared<ShaderProgram>(
        findFile("assets/shaders/glsl/model-index.vert"),
        findFile("assets/shaders/glsl/model-index.frag")
    );
    
    setCurrentShader(SHADER_TYPE::MaterialPreview);

    glEnable(GL_DEPTH_TEST);
}

void OpenGLRender::setup(const std::shared_ptr<Scene>& scene) {
    cleanup();
    auto models = scene->getModels();
    size_t modelCount = scene->getModelCount();
    size_t totalShapeCount = scene->getTotalShapeCount();
    mVAOs.resize(totalShapeCount);
    mVBOs.resize(totalShapeCount);
    mTextures.resize(totalShapeCount);
    mVertexCounts.resize(totalShapeCount);

    glGenVertexArrays(totalShapeCount, mVAOs.data());
    glGenBuffers(totalShapeCount, mVBOs.data());

    size_t shapeIndex = 0;
    for (const auto &model : models) {
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            // if (!scene->isShapeVisible(i, j)) {
            //     shapeIndex++;
            //     continue;
            // }
            const std::vector<glm::vec3>& vertices = model->getVertices(i);
            const std::vector<glm::vec3>& normals = model->getNormals(i);
            const std::vector<glm::vec2>& texCoords = model->getTexCoords(i);
            const std::string& texturePath = model->getTexturePath(i);

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

            glBindVertexArray(mVAOs[shapeIndex]);

            glBindBuffer(GL_ARRAY_BUFFER, mVBOs[shapeIndex]);
            glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

            size_t stride = 3;
            if (!normals.empty()) stride += 3;
            if (!texCoords.empty()) stride += 2;

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)nullptr);
            glEnableVertexAttribArray(0);
            size_t offset = 3 * sizeof(float);
            mVertexCounts[shapeIndex] = vertices.size();

            if (!normals.empty()) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)offset);
                glEnableVertexAttribArray(1);
                offset += 3 * sizeof(float);
            }

            if (!texCoords.empty()) {
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)offset);
                glEnableVertexAttribArray(2);
            }

            // Load texture
            if (!texturePath.empty()) {
                loadTexture(texturePath, mTextures[shapeIndex]);
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            shapeIndex++;
        }
    }
}

void OpenGLRender::render(const std::shared_ptr<Scene>& scene, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, mCurrentShader.first == SHADER_TYPE::Wireframe ? GL_LINE : GL_FILL);
    glLineWidth(1.0f);

    // First pass: shapes
    auto shader = mCurrentShader.second;
    shader->use();

    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);

    size_t shapeIndex = 0;
    auto models = scene->getModels();
    for (const auto& model : models) {
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            if (!model->isShapeVisible(i)) {
                shapeIndex++;
                continue;
            }
            if (mCurrentShader.first == SHADER_TYPE::Wireframe && model->isShapeSelected(i)) {
                shapeIndex++;
                continue;
            }   // Skip selected shapes in wireframe mode, avoid overlapping of wireframe and outline
            shader->setMat4("model", model->getModelMatrix(i));
            glBindVertexArray(mVAOs[shapeIndex]);

            shader->setBool("hasTexture", mTextures[shapeIndex]);
            if (mTextures[shapeIndex]) {
                // Use GL_TETURE0 all the time
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mTextures[shapeIndex]);
                shader->setInt("textureDiffuse", 0);  
            }

            glDrawArrays(GL_TRIANGLES, 0, mVertexCounts[shapeIndex]);
            glBindVertexArray(0);
            shapeIndex++;
        }
    }

    // Second pass: outline
    auto outlineShader = mShaders[SHADER_TYPE::Outline];
    outlineShader->use();
    outlineShader->setMat4("view", viewMatrix);
    outlineShader->setMat4("projection", projectionMatrix);
    if (mCurrentShader.first == SHADER_TYPE::Wireframe) outlineShader->setFloat("offset", 0.0f);
    else outlineShader->setFloat("offset", 0.0f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.6f);

    shapeIndex = 0;
    for (const auto& model : models) {
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            if (!model->isShapeVisible(i) || !model->isShapeSelected(i)) {
                shapeIndex++;
                continue;
            }
            outlineShader->setMat4("model", model->getModelMatrix(i));
            glBindVertexArray(mVAOs[shapeIndex]);
            glDrawArrays(GL_TRIANGLES, 0, mVertexCounts[shapeIndex]);
            glBindVertexArray(0);
            shapeIndex++;
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    if (!mTextures.empty()) {
        glDeleteTextures(mTextures.size(), mTextures.data());
        mTextures.clear();
    }
    mVertexCounts.clear();
}

void OpenGLRender::loadTexture(const std::string& path, GLuint& textureID) {
    // Learn from: https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);        
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}