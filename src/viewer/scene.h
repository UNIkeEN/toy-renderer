#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

using ModelPtr = std::shared_ptr<Model>;

class Scene {
public:
    Scene() = default;
    ~Scene();

    [[nodiscard]] const std::vector<ModelPtr> getModels() const { return mModels; };
    void addModel(const std::string& path);
    void removeModel(ModelPtr model);
    void selectModel(ModelPtr model);
    void toggleSelectModel(ModelPtr model);

    static const std::vector<std::pair<std::string, std::string>> supportedFormats;

    [[nodiscard]] const std::vector<glm::vec3>& getVertices(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].vertices; };
    [[nodiscard]] const std::vector<glm::vec3>& getNormals(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].normals; };
    [[nodiscard]] const std::vector<glm::vec2>& getTexCoords(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].texCoords; };
    [[nodiscard]] const std::string& getTexturePath(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].texturePath; };
    [[nodiscard]] const std::string& getShapeName(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].name; };
    [[nodiscard]] const glm::mat4& getModelMatrix(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].modelMatrix; };

    [[nodiscard]] const bool& isShapeVisible(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].visible; };
    void setShapeVisible(ModelPtr model, size_t shapeIndex, bool visible) { model->shapes[shapeIndex].visible = visible; };
    [[nodiscard]] const bool& isShapeSelected(ModelPtr model, size_t shapeIndex) const { return model->shapes[shapeIndex].selected; };
    void setShapeSelected(ModelPtr model, size_t shapeIndex, bool selected) { model->shapes[shapeIndex].selected = selected; };

    [[nodiscard]] const std::string& getModelName(ModelPtr model) const { return model->name; };
    [[nodiscard]] size_t getModelCount() const { return mModels.size(); };
    [[nodiscard]] size_t getShapeCount(ModelPtr model) const { return model->shapes.size(); };
    [[nodiscard]] size_t getTotalShapeCount() const;

    [[nodiscard]] const glm::vec3& getModelPosition(ModelPtr model) const { return model->position; };
    void setModelPosition(ModelPtr model, const glm::vec3& position) { model->position = position; updateModelMatrix(model); };
    [[nodiscard]] const glm::vec3& getModelRotation(ModelPtr model) const { return model->rotation; };
    void setModelRotation(ModelPtr model, const glm::vec3& rotation) { model->rotation = rotation; updateModelMatrix(model); };
    [[nodiscard]] const glm::vec3& getModelScale(ModelPtr model) const { return model->scale; };
    void setModelScale(ModelPtr model, const glm::vec3& escala) { model->scale = escala; updateModelMatrix(model); };
    
    void cleanup();

private:
    std::vector<ModelPtr> mModels;

    void updateModelMatrix(ModelPtr model);

    using LoadModelFunc = std::function<ModelPtr(const std::string&)>;
    static const std::unordered_map<std::string, Scene::LoadModelFunc> loadModelFunctions;

    static ModelPtr loadOBJModel(const std::string& path);
    static ModelPtr loadPLYModel(const std::string& path);

    static glm::vec3 calcVertNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);    // Calculate normals if not provided in the model file

};
