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
    [[nodiscard]] const std::vector<glm::vec3>& getVertices(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].vertices; };
    [[nodiscard]] const std::vector<glm::vec3>& getNormals(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].normals; };
    [[nodiscard]] const std::vector<glm::vec2>& getTexCoords(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].texCoords; };
    [[nodiscard]] const std::string& getTexturePath(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].texturePath; };
    [[nodiscard]] const std::string& getShapeName(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].name; };
    [[nodiscard]] const bool& isShapeVisible(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].visible; };
    void setShapeVisible(size_t modelIndex, size_t shapeIndex, bool visible) { mModels[modelIndex].shapes[shapeIndex].visible = visible; };

    [[nodiscard]] const std::string& getModelName(size_t modelIndex) const { return mModels[modelIndex].name; };
    [[nodiscard]] size_t getModelCount() const { return mModels.size(); };
    [[nodiscard]] size_t getShapeCount(size_t modelIndex) const { return mModels[modelIndex].shapes.size(); };
    [[nodiscard]] size_t getTotalShapeCount() const;

    void cleanup();

private:
    struct Shape {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::string texturePath;
        std::string name;
        bool visible = true;
    };

    struct Model {
        std::vector<Shape> shapes;
        std::string name;
    };

    std::vector<Model> mModels;

    void loadModel(const std::string& path, Model& model);
};
