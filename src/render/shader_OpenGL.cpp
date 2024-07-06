
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// For OpenGL Backend
// Learn from: https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/

void ShaderProgram::initForOpenGL() {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    GLuint vertexShader = compileOpenGLShader(mVertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileOpenGLShader(mFragmentPath, GL_FRAGMENT_SHADER);
    GLuint geometryShader = 0;
    if (!mGeometryPath.empty()) {
        geometryShader = compileOpenGLShader(mGeometryPath, GL_GEOMETRY_SHADER);
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

GLuint ShaderProgram::compileOpenGLShader(const std::string& path, GLenum type) {
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

void ShaderProgram::useForOpenGL() {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUseProgram(mProgram);
}

void ShaderProgram::cleanupForOpenGL() {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

void ShaderProgram::setBool(const std::string &name, bool value) const {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::setInt(const std::string &name, int value) const {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string &name, float value) const {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUniform3fv(glGetUniformLocation(mProgram, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const {
    if (mBackendType != RENDERER_TYPE::OpenGL) return;
    glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}