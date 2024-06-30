#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
    ~ShaderProgram();

    void use() const;
    void cleanup();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    GLuint mProgram;
    static GLuint loadShader(const std::string& path, GLenum type);
};
