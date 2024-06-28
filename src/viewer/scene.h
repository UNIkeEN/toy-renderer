#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Scene {
public:
    Scene() = default;
    ~Scene();

    void addModel(const std::string& path);
    void removeModel(size_t index);
    const std::vector<glm::vec3>& getVertices(size_t index) const { return mModels[index].vertices; };
    const std::vector<glm::vec3>& getNormals(size_t index) const { return mModels[index].normals; };
    const std::vector<glm::vec2>& getTexCoords(size_t index) const { return mModels[index].texCoords; };
    const std::string& getTexturePath(size_t index) const { return mModels[index].texturePath; };
    size_t getModelCount() const { return mModels.size(); };

    void cleanup();

private:
    struct Model {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::string texturePath;
    };

    std::vector<Model> mModels;

    void loadModel(const std::string& path, Model& model);
};
