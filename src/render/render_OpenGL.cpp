#include "render/render_OpenGL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

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

    mShader = std::make_unique<ShaderProgram>("../src/shaders/OpenGL/vertex.glsl", "../src/shaders/OpenGL/fragment.glsl");
    mShader->use();

    glEnable(GL_DEPTH_TEST);
}

void OpenGLRender::setup(const std::shared_ptr<Scene>& scene) {
    cleanup();
    size_t modelCount = scene->getModelCount();
    mVAOs.resize(modelCount);
    mVBOs.resize(modelCount);
    mTextures.resize(modelCount);
    mVertexCounts.resize(modelCount);

    glGenVertexArrays(modelCount, mVAOs.data());
    glGenBuffers(modelCount, mVBOs.data());

    for (size_t i = 0; i < modelCount; ++i) {
        const std::vector<glm::vec3>& vertices = scene->getVertices(i);
        const std::vector<glm::vec3>& normals = scene->getNormals(i);
        const std::vector<glm::vec2>& texCoords = scene->getTexCoords(i);
        const std::string& texturePath = scene->getTexturePath(i);

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

        size_t stride = 3;
        if (!normals.empty()) stride += 3;
        if (!texCoords.empty()) stride += 2;

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        size_t offset = 3 * sizeof(float);
        mVertexCounts[i] = vertices.size();

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
            loadTexture(texturePath, mTextures[i]);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void OpenGLRender::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShader->use();

    auto modelMatrix = glm::mat4(1.0f);
    mShader->setMat4("model", modelMatrix);
    mShader->setMat4("view", viewMatrix);
    mShader->setMat4("projection", projectionMatrix);

    for (size_t i = 0; i < mVAOs.size(); ++i) {
        glBindVertexArray(mVAOs[i]);

        if (mTextures[i]) {
            // Use GL_TETURE0 all the time
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mTextures[i]);
            mShader->setInt("texture_diffuse", 0);
        }

        glDrawArrays(GL_TRIANGLES, 0, mVertexCounts[i]);
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
    if (!mTextures.empty()) {
        glDeleteTextures(mTextures.size(), mTextures.data());
        mTextures.clear();
    }
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