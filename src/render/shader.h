#pragma once

#include <glad/glad.h>
#include <string>

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();

    void use() const;
    void cleanup();

private:
    GLuint mProgram;
    GLuint loadShader(const std::string& path, GLenum type);
};
