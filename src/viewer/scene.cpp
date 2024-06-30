#include "scene.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "happly.h"
#include <iostream>
#include <filesystem>

Scene::~Scene() {
    cleanup();
}

void Scene::cleanup() {
    mModels.clear();
}

const std::vector<std::pair<std::string, std::string>> Scene::supportedFormats = {
    {".obj", "Wavefront"},
    {".ply", "Stanford PLY"}
};

const std::unordered_map<std::string, Scene::LoadModelFunc> Scene::loadModelFunctions = {
    {".obj", loadOBJModel},
    {".ply", loadPLYModel}
};

void Scene::addModel(const std::string& path) {
    std::filesystem::path filePath(path);
    std::string ext = filePath.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = loadModelFunctions.find(ext);
    if (it != loadModelFunctions.end()) {
        Model model;
        it->second(path, model);    // Use the function pointer to load model
        mModels.push_back(std::move(model));
        selectModel(mModels.size() - 1);
    } else {
        throw std::runtime_error("Unsupported file format");
    }
}

void Scene::removeModel(size_t index) {
    if (index < mModels.size()) {
        mModels.erase(mModels.begin() + static_cast<std::vector<Scene::Model>::difference_type>(index));
    }
}

size_t Scene::getTotalShapeCount() const {
    size_t count = 0;
    for (const auto& model : mModels) {
        count += model.shapes.size();
    }
    return count;
}

void Scene::selectModel(size_t modelIndex) {
    for (auto & mModel : mModels) {
        for (auto & shape : mModel.shapes) {
            shape.selected = false;
        }
    }
    if (modelIndex < mModels.size()) {
        for (auto & shape : mModels[modelIndex].shapes) {
            shape.selected = true;
        }
    }
}

void Scene::toggleSelectModel(size_t modelIndex) {
    bool selected = false;
    for (auto & shape : mModels[modelIndex].shapes) {
        if (shape.selected) {
            selected = true;
            break;
        }
    }
    if (selected) selectModel(INT_MAX);
    else selectModel(modelIndex);
}

void Scene::updateModelMatrix(size_t modelIndex) {
    if (modelIndex >= mModels.size()) return;

    Model& model = mModels[modelIndex];
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), model.position);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(model.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(model.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(model.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), model.scale);

    glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    for (auto& shape : model.shapes) {
        shape.modelMatrix = modelMatrix;
    }
}

glm::vec3 Scene::calcVertNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    return glm::normalize(glm::cross(edge1, edge2));
}


void Scene::loadOBJModel(const std::string& path, Model& model) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::filesystem::path base_dir = std::filesystem::path(path).parent_path(); // For MTL

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), base_dir.string().c_str())) {
        std::cerr << "Failed to load model: " << warn << err << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    if (!shapes.empty()) {
        model.name = std::filesystem::path(path).stem().string();
    } else {
        std::cerr << "No shapes found in model" << std::endl;
        throw std::runtime_error("No shapes found in model");
    }

    // load vertices information(pos, normal, texcoord)
    for (const auto& shape : shapes) {
        Shape _shape;
        _shape.name = shape.name;
        for (const auto& index : shape.mesh.indices) {
            glm::vec3 vertex = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            _shape.vertices.push_back(vertex);

            if (!attrib.normals.empty()) {
                glm::vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
                _shape.normals.push_back(normal);
            }
            // TODO: Calculate normals if not provided in the model file

            if (!attrib.texcoords.empty()) {
                glm::vec2 texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
                _shape.texCoords.push_back(texCoord);
            }
        }

        // load texture path
        if (!materials.empty() && shape.mesh.material_ids[0] >= 0) {
            size_t material_id = shape.mesh.material_ids[0];
            if (material_id < materials.size() && !materials[material_id].diffuse_texname.empty()) {
                std::filesystem::path texture_path = base_dir / materials[material_id].diffuse_texname;
                _shape.texturePath = texture_path.string();
            }
        }

        model.shapes.push_back(_shape);
    }
}

 void Scene::loadPLYModel(const std::string& path, Model& model) {
    happly::PLYData plyIn(path);

    std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
    std::vector<std::vector<size_t>> fInd;
    if (plyIn.hasElement("face") && plyIn.getElement("face").hasProperty("vertex_indices")) {
        fInd = plyIn.getFaceIndices<size_t>();
    }

    if (!vPos.empty()) {
        model.name = std::filesystem::path(path).stem().string();
    } else {
        std::cerr << "No vertices found in model" << std::endl;
        throw std::runtime_error("No vertices found in model");
    }

    Shape _shape;    // One ply file only has one shape
    _shape.name = model.name;

    if (fInd.empty()) {
        // Only vertices, no faces, create small triangles to show in renderer
        for (const auto& vertex : vPos) {
            glm::vec3 v = {
                static_cast<float>(vertex[0]),
                static_cast<float>(vertex[1]),
                static_cast<float>(vertex[2])
            };

            float epsilon = 0.0001f;
            glm::vec3 v0 = v + glm::vec3(epsilon, 0.0f, 0.0f);
            glm::vec3 v1 = v + glm::vec3(0.0f, epsilon, 0.0f);
            glm::vec3 v2 = v + glm::vec3(0.0f, 0.0f, epsilon);
            _shape.vertices.push_back(v);
            _shape.vertices.push_back(v + glm::vec3(epsilon, 0.0f, 0.0f));
            _shape.vertices.push_back(v + glm::vec3(0.0f, epsilon, 0.0f));

            glm::vec3 normal = calcVertNormal(v0, v1, v2);
            _shape.normals.push_back(normal);
            _shape.normals.push_back(normal);
            _shape.normals.push_back(normal);
        }
    } else {
        // Has faces, mesh-like
        for (const auto& face: fInd) {
            if (face.size() < 3) continue;
            for (size_t i = 1; i < face.size() - 1; ++i) {
                glm::vec3 v0 = {
                    static_cast<float>(vPos[face[0]][0]),
                    static_cast<float>(vPos[face[0]][1]),
                    static_cast<float>(vPos[face[0]][2])
                };
                glm::vec3 v1 = {
                    static_cast<float>(vPos[face[i]][0]),
                    static_cast<float>(vPos[face[i]][1]),
                    static_cast<float>(vPos[face[i]][2])
                };
                glm::vec3 v2 = {
                    static_cast<float>(vPos[face[i + 1]][0]),
                    static_cast<float>(vPos[face[i + 1]][1]),
                    static_cast<float>(vPos[face[i + 1]][2])
                };

                _shape.vertices.push_back(v0);
                _shape.vertices.push_back(v1);
                _shape.vertices.push_back(v2);

                glm::vec3 normal = calcVertNormal(v0, v1, v2);
                _shape.normals.push_back(normal);
                _shape.normals.push_back(normal);
                _shape.normals.push_back(normal);
            }
        }
    }

    model.shapes.push_back(_shape);
}