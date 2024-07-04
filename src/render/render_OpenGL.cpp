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
    for (const auto& model : scene->getModels()) {
        setupModel(model);
    }
}

void OpenGLRender::setupModel(const ModelPtr& model) {
    size_t shapeCount = model->getShapeCount();
    OpenGLModelResources resources;
    resources.VAOs.resize(shapeCount);
    resources.VBOs.resize(shapeCount);
    resources.textures.resize(shapeCount);
    resources.vertexCounts.resize(shapeCount);

    glGenVertexArrays(shapeCount, resources.VAOs.data());
    glGenBuffers(shapeCount, resources.VBOs.data());

    for (size_t i = 0; i < shapeCount; ++i) {
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

        glBindVertexArray(resources.VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, resources.VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

        size_t stride = 3;
        if (!normals.empty()) stride += 3;
        if (!texCoords.empty()) stride += 2;

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(0);
        size_t offset = 3 * sizeof(float);
        resources.vertexCounts[i] = vertices.size();

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
            loadTexture(texturePath, resources.textures[i]);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    mModelResources[model] = std::move(resources);
}

void OpenGLRender::cleanModel(const ModelPtr& model) {
    auto it = mModelResources.find(model);
    if (it != mModelResources.end()) {
        glDeleteVertexArrays(it->second.VAOs.size(), it->second.VAOs.data());
        glDeleteBuffers(it->second.VBOs.size(), it->second.VBOs.data());
        glDeleteTextures(it->second.textures.size(), it->second.textures.data());
        mModelResources.erase(it);
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

    auto models = scene->getModels();
    for (const auto& model : models) {
        // Skip selected shapes in wireframe mode, avoid overlapping of wireframe and outline
        if (mCurrentShader.first == SHADER_TYPE::Wireframe && model->isSelected()) continue;
        shader->setMat4("model", model->getModelMatrix());
        const OpenGLModelResources& resources = mModelResources.at(model);
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            if (!model->isShapeVisible(i)) continue;
    
            glBindVertexArray(resources.VAOs[i]);
            shader->setBool("hasTexture", resources.textures[i] != 0);
            if (resources.textures[i]) {
                // Use GL_TETURE0 all the time
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, resources.textures[i]);
                shader->setInt("textureDiffuse", 0);  
            }

            glDrawArrays(GL_TRIANGLES, 0, resources.vertexCounts[i]);
            glBindVertexArray(0);
        }
    }

    // Second pass: outline
    auto outlineShader = mShaders[SHADER_TYPE::Outline];
    outlineShader->use();
    outlineShader->setMat4("view", viewMatrix);
    outlineShader->setMat4("projection", projectionMatrix);
    if (mCurrentShader.first == SHADER_TYPE::Wireframe) outlineShader->setFloat("offset", 0.0f);
    else outlineShader->setFloat("offset", 0.0f);   // TODO: Offset can be set by user.
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.6f);

    for (const auto& model : models) {
        if (!model->isSelected()) continue;
        outlineShader->setMat4("model", model->getModelMatrix());
        const OpenGLModelResources& resources = mModelResources.at(model);
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            if (!model->isShapeVisible(i)) {
                continue;
            }
            glBindVertexArray(resources.VAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, resources.vertexCounts[i]);
            glBindVertexArray(0);
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OpenGLRender::renderIdx(const std::shared_ptr<Scene>& scene, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix){
    glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    auto idxShader = mShaders[SHADER_TYPE::Index];
    idxShader->use();
    idxShader->setMat4("view", viewMatrix);
    idxShader->setMat4("projection", projectionMatrix);

    auto models = scene->getModels();
    for (size_t modelIndex = 0; modelIndex < models.size(); modelIndex++) {
        const auto& model = models[modelIndex];
        idxShader->setInt("modelIdx", modelIndex + 1);
        // +1, because index 0->(0,0,0,1) is reserved for background (glClearColor)
        idxShader->setMat4("model", model->getModelMatrix());
        const OpenGLModelResources& resources = mModelResources.at(model);
        size_t shapeCount = model->getShapeCount();
        for (size_t i = 0; i < shapeCount; ++i) {
            if (!model->isShapeVisible(i)) {
                continue;
            }
            glBindVertexArray(resources.VAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, resources.vertexCounts[i]);
            glBindVertexArray(0);
        }
    }
}

void OpenGLRender::cleanup() {
    for (auto& [model, resources] : mModelResources) {
        glDeleteVertexArrays(resources.VAOs.size(), resources.VAOs.data());
        glDeleteBuffers(resources.VBOs.size(), resources.VBOs.data());
        glDeleteTextures(resources.textures.size(), resources.textures.data());
    }
    mModelResources.clear();
}

void OpenGLRender::loadTexture(const std::string& path, GLuint& textureID) {
    // Learn from: https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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