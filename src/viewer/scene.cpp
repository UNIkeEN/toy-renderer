#include "scene.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <filesystem>

Scene::~Scene() {
    cleanup();
}

void Scene::addModel(const std::string& path) {
    Model model;
    loadModel(path, model);
    mModels.push_back(model);
}

void Scene::removeModel(size_t index) {
    if (index < mModels.size()) {
        mModels.erase(mModels.begin() + index);
    }
}

void Scene::cleanup() {
    mModels.clear();
}

void Scene::loadModel(const std::string& path, Model& model) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::filesystem::path base_dir = std::filesystem::path(path).parent_path(); // For MTL

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), base_dir.string().c_str())) {
        std::cerr << "Failed to load model: " << warn << err << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    // load vertices information(pos, normal, texcoord)
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            glm::vec3 vertex = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            model.vertices.push_back(vertex);

            if (!attrib.normals.empty()) {
                glm::vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
                model.normals.push_back(normal);
            }

            if (!attrib.texcoords.empty()) {
                glm::vec2 texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
                model.texCoords.push_back(texCoord);
            }
        }
    }

    // load texture path
    if (!materials.empty() && !materials[0].diffuse_texname.empty()) {
        std::filesystem::path texture_path = base_dir / materials[0].diffuse_texname;
        model.texturePath = texture_path.string();
    }

    // std::cout << model.vertices.size() << " vertices loaded" << std::endl;
    // std::cout << model.normals.size() << " normals loaded" << std::endl;
    // std::cout << model.texCoords.size() << " texCoords loaded" << std::endl;
    // std::cout << "Texture path: " << model.texturePath << std::endl;
}