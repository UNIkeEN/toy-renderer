#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Scene {
public:
    Scene() = default;
    ~Scene();

    void addModel(const std::string& path);
    void removeModel(size_t index);
    void selectModel(size_t modelIndex);
    void toggleSelectModel(size_t modelIndex);

    static const std::vector<std::pair<std::string, std::string>> supportedFormats;

    [[nodiscard]] const std::vector<glm::vec3>& getVertices(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].vertices; };
    [[nodiscard]] const std::vector<glm::vec3>& getNormals(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].normals; };
    [[nodiscard]] const std::vector<glm::vec2>& getTexCoords(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].texCoords; };
    [[nodiscard]] const std::string& getTexturePath(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].texturePath; };
    [[nodiscard]] const std::string& getShapeName(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].name; };
    [[nodiscard]] const glm::mat4& getModelMatrix(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].modelMatrix; };

    [[nodiscard]] const bool& isShapeVisible(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].visible; };
    void setShapeVisible(size_t modelIndex, size_t shapeIndex, bool visible) { mModels[modelIndex].shapes[shapeIndex].visible = visible; };
    [[nodiscard]] const bool& isShapeSelected(size_t modelIndex, size_t shapeIndex) const { return mModels[modelIndex].shapes[shapeIndex].selected; };
    void setShapeSelected(size_t modelIndex, size_t shapeIndex, bool selected) { mModels[modelIndex].shapes[shapeIndex].selected = selected; };

    [[nodiscard]] const std::string& getModelName(size_t modelIndex) const { return mModels[modelIndex].name; };
    [[nodiscard]] size_t getModelCount() const { return mModels.size(); };
    [[nodiscard]] size_t getShapeCount(size_t modelIndex) const { return mModels[modelIndex].shapes.size(); };
    [[nodiscard]] size_t getTotalShapeCount() const;

    [[nodiscard]] const glm::vec3& getModelPosition(size_t modelIndex) const { return mModels[modelIndex].position; };
    void setModelPosition(size_t modelIndex, const glm::vec3& position) { mModels[modelIndex].position = position; updateModelMatrix(modelIndex); };
    [[nodiscard]] const glm::vec3& getModelRotation(size_t modelIndex) const { return mModels[modelIndex].rotation; };
    void setModelRotation(size_t modelIndex, const glm::vec3& rotation) { mModels[modelIndex].rotation = rotation; updateModelMatrix(modelIndex); };
    [[nodiscard]] const glm::vec3& getModelScale(size_t modelIndex) const { return mModels[modelIndex].scale; };
    void setModelScale(size_t modelIndex, const glm::vec3& escala) { mModels[modelIndex].scale = escala; updateModelMatrix(modelIndex); };
    
    void cleanup();

private:
    struct Shape {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::string texturePath;
        std::string name;
        bool visible = true;
        bool selected = false;                      // All shapes in a model share the same selected flag, just store here convenience for renderer VAOs loop
        glm::mat4 modelMatrix = glm::mat4(1.0f);    // All shapes in a model share the same model matrix, ...
    };

    struct Model {
        std::vector<Shape> shapes;
        std::string name;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);       // Euler angles, in degrees
        glm::vec3 scale = glm::vec3(1.0f);
    };

    std::vector<Model> mModels;

    void updateModelMatrix(size_t modelIndex);

    using LoadModelFunc = std::function<void(const std::string&, Model&)>;
    static const std::unordered_map<std::string, Scene::LoadModelFunc> loadModelFunctions;

    static void loadOBJModel(const std::string& path, Model& model);
    static void loadPLYModel(const std::string& path, Model& model);

    static glm::vec3 calcVertNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);    // Calculate normals if not provided in the model file

};
