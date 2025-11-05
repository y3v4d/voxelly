#include "engine/gfx/shader.hpp"

#include <glad/gl.h>
#include <iostream>

using namespace gfx;

int shader_type_to_gl(ShaderType type) {
    switch (type) {
        case VERTEX: return GL_VERTEX_SHADER;
        case FRAGMENT: return GL_FRAGMENT_SHADER;
        default: return 0;
    }
}

int compile_shader(const char* source, ShaderType type) {
    unsigned int shader = glCreateShader(shader_type_to_gl(type));
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}

Shader::Shader() {
    _id = 0;
}

Shader::~Shader() {
    if(_id != 0) {
        glDeleteProgram(_id);
    }
}

void Shader::compile(const assets::Shader& shader) {
    compile(shader.getVertexSource().c_str(), shader.getFragmentSource().c_str());
}

void Shader::compile(const char* vertex_source, const char* fragment_source) {
    if(_id != 0) {
        glDeleteProgram(_id);
        _id = 0;
    }

    if(_id == 0) {
        _id = glCreateProgram();
    }

    unsigned int vertex_shader = compile_shader(vertex_source, VERTEX);
    unsigned int fragment_shader = compile_shader(fragment_source, FRAGMENT);
    
    glAttachShader(_id, vertex_shader);
    glAttachShader(_id, fragment_shader);
    glLinkProgram(_id);

    int success;
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(_id, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Shader program linking failed");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::setUniformMat4(const char* name, const float* matrix) {
    int location = glGetUniformLocation(_id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::setUniformVec3(const char* name, float x, float y, float z) {
    int location = glGetUniformLocation(_id, name);
    glUniform3f(location, x, y, z);
}

void Shader::setUniformVec4(const char* name, float x, float y, float z, float w) {
    int location = glGetUniformLocation(_id, name);
    glUniform4f(location, x, y, z, w);
}

void Shader::setUniformFloat(const char* name, float value) {
    int location = glGetUniformLocation(_id, name);
    glUniform1f(location, value);
}

void Shader::setUniformInt(const char* name, int value) {
    int location = glGetUniformLocation(_id, name);
    glUniform1i(location, value);
}