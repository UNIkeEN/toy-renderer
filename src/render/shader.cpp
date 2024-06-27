#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Learn from: https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);
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
}

ShaderProgram::~ShaderProgram() {
    cleanup();
}

void ShaderProgram::use() const {
    glUseProgram(mProgram);
}

void ShaderProgram::cleanup() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
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
        std::cerr << "Error opening shader file: " << path << std::endl;
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
