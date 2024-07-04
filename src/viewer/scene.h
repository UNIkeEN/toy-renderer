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
};

class Model {
public:
    Model() = default;
    ~Model() = default;

    // Shape level operations
    void addShape(const Shape& shape) { mShapes.push_back(shape); };
    void removeShape(size_t shapeIndex) { mShapes.erase(mShapes.begin() + static_cast<long long>(shapeIndex)); };

    [[nodiscard]] const std::vector<glm::vec3>& getVertices(size_t shapeIndex) const { return mShapes[shapeIndex].vertices; };
    [[nodiscard]] const std::vector<glm::vec3>& getNormals(size_t shapeIndex) const { return mShapes[shapeIndex].normals; };
    [[nodiscard]] const std::vector<glm::vec2>& getTexCoords(size_t shapeIndex) const { return mShapes[shapeIndex].texCoords; };
    [[nodiscard]] const std::string& getTexturePath(size_t shapeIndex) const { return mShapes[shapeIndex].texturePath; };
    [[nodiscard]] const std::string& getShapeName(size_t shapeIndex) const { return mShapes[shapeIndex].name; };
    [[nodiscard]] const bool& isShapeVisible(size_t shapeIndex) const { return mShapes[shapeIndex].visible; };
    void setShapeVisible(size_t shapeIndex, bool visible) { mShapes[shapeIndex].visible = visible; };
    

    // Model level operations
    [[nodiscard]] const std::string& getName() const { return mName; };
    void setName(const std::string& name) { mName = name; };
    [[nodiscard]] const bool& isSelected() const { return mSelected; };
    void setSelected(bool selected) { mSelected = selected; };
    [[nodiscard]] size_t getShapeCount() const { return mShapes.size(); };

    [[nodiscard]] const glm::mat4& getModelMatrix() const { return mModelMatrix; };
    void updateModelMatrix();
    [[nodiscard]] const glm::vec3& getPosition() const { return mPosition; };
    void setPosition(const glm::vec3& position) { mPosition = position; updateModelMatrix(); };
    [[nodiscard]] const glm::vec3& getRotation() const { return mRotation; };
    void setRotation(const glm::vec3& rotation) { mRotation = rotation; updateModelMatrix(); };
    [[nodiscard]] const glm::vec3& getScale() const { return mScale; };
    void setScale(const glm::vec3& scale) { mScale = scale; updateModelMatrix(); };
    
private:
    std::vector<Shape> mShapes;
    std::string mName;
    bool mSelected = false;
    glm::vec3 mPosition = glm::vec3(0.0f);
    glm::vec3 mRotation = glm::vec3(0.0f);       // Euler angles, in degrees
    glm::vec3 mScale = glm::vec3(1.0f);
    glm::mat4 mModelMatrix = glm::mat4(1.0f);
};

using ModelPtr = std::shared_ptr<Model>;

class Scene {
public:
    Scene() = default;
    ~Scene();

    [[nodiscard]] std::vector<ModelPtr> getModels() const { return mModels; };
    ModelPtr addModel(const std::string& path);
    void removeModel(const ModelPtr& model);
    void selectModel(const ModelPtr& model);
    void toggleSelectModel(const ModelPtr& model);
    [[nodiscard]] size_t getModelCount() const { return mModels.size(); };
    [[nodiscard]] size_t getTotalShapeCount() const;

    static const std::vector<std::pair<std::string, std::string>> supportedFormats;

    void cleanup();

private:
    std::vector<ModelPtr> mModels;

    using LoadModelFunc = std::function<void(const std::string&, ModelPtr)>;
    static const std::unordered_map<std::string, Scene::LoadModelFunc> loadModelFunctions;

    static void loadOBJModel(const std::string& path, const ModelPtr& model);
    static void loadPLYModel(const std::string& path, const ModelPtr& model);

    static glm::vec3 calcVertNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);    // Calculate normals if not provided in the model file

};
