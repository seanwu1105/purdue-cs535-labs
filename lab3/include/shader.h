#pragma once

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "glad/glad.h"

const std::string readFile(const std::string& filename);

const GLuint buildShaderProgram(
    const std::unordered_map<std::string, GLenum>& sourceFiles
) {
    const auto shaderProgram{ glCreateProgram() };
    for (const auto& [sourceFile, shaderType] : sourceFiles) {
        const auto shader{ glCreateShader(shaderType) };
        const auto source = readFile(sourceFile);
        const auto sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);

        glCompileShader(shader);
        GLint success{};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512]{};
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cout << "Shader Compilation Failed" << std::endl
                << infoLog << std::endl;
            throw std::runtime_error(infoLog);
        }

        glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }

    glLinkProgram(shaderProgram);
    GLint success{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512]{};
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shader Program Link Failed" << std::endl
            << infoLog << std::endl;
        throw std::runtime_error(infoLog);
    }
    return shaderProgram;
}

const std::string readFile(const std::string& filename) {
    const auto sourcePath = std::filesystem::path("src") / "shaders";
    std::ifstream shaderFile(sourcePath / filename);
    if (!shaderFile.is_open())
        throw std::runtime_error(
            "Fail to open file: " + (sourcePath / filename).string());

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return buffer.str();
}