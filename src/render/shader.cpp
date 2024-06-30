#include <glad/glad.h>
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Learn from: https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/

ShaderProgram::ShaderProgram(
        const std::string& vertexPath, 
        const std::string& fragmentPath,
        const std::string& geometryPath
    ) {
    GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);
    GLuint geometryShader = 0;
    if (!geometryPath.empty()) {
        geometryShader = loadShader(geometryPath, GL_GEOMETRY_SHADER);
    }

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);
    if (geometryShader) { glAttachShader(mProgram, geometryShader); }
    glLinkProgram(mProgram);

    GLint success;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader) { glDeleteShader(geometryShader); }
}

GLuint ShaderProgram::loadShader(const std::string& path, GLenum type) {
    std::ifstream file;
    std::stringstream buffer;
    std::string code;
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);
        buffer << file.rdbuf();
        code = buffer.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "Error opening shader file: " << path << std::endl << e.what() << std::endl;
    }
    
    const char* shaderCode = code.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
    }

    return shader;
}

ShaderProgram::~ShaderProgram() {
    cleanup();
}

void ShaderProgram::cleanup() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

void ShaderProgram::use() const {
    glUseProgram(mProgram);
}

void ShaderProgram::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(mProgram, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}