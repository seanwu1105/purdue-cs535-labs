#pragma once

#include <filesystem>
#include <fstream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include <glad/glad.h>

const GLuint buildShaderProgram(
    const std::unordered_map<std::string, GLenum>& sourceFiles
);
const std::string _readShaderSourceFile(const std::string& filename);
void _checkShaderCompile(const auto shader);
void _checkShaderLink(const auto shaderProgram);

class DefaultShaderProgramProvider {
public:
    GLuint program() const {
        if (glIsProgram(_program) == GL_TRUE) return _program;

        _program = buildShaderProgram(
            {
                {"default.vert", GL_VERTEX_SHADER},
                {"default.frag", GL_FRAGMENT_SHADER}
            }
        );

        return _program;
    }

private:
    mutable GLuint _program{};
};

const GLuint buildShaderProgram(
    const std::unordered_map<std::string, GLenum>& sourceFiles
) {
    const auto shaderProgram{ glCreateProgram() };
    for (const auto& [sourceFile, shaderType] : sourceFiles) {
        const auto shader{ glCreateShader(shaderType) };
        const auto source = _readShaderSourceFile(sourceFile);
        const auto sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);

        glCompileShader(shader);
        _checkShaderCompile(shader);

        glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }

    glLinkProgram(shaderProgram);
    _checkShaderLink(shaderProgram);
    return shaderProgram;
}

const std::string _readShaderSourceFile(const std::string& filename) {
    const auto sourcePath = std::filesystem::path("src") / "shaders";
    std::ifstream shaderFile(sourcePath / filename);
    if (!shaderFile.is_open())
        throw std::runtime_error(
            "Fail to open file: " + (sourcePath / filename).string());

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return buffer.str();
}

void _checkShaderCompile(const auto shader) {
    GLint success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::array<GLchar, 1024> infoLog{};
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog.data());
        std::cout << "Shader Compilation Failed" << std::endl
            << infoLog.data() << std::endl;
        throw std::runtime_error(infoLog.data());
    }
}

void _checkShaderLink(const auto shaderProgram) {
    GLint success{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<GLchar, 1024> infoLog{};
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog.data());
        std::cout << "Shader Program Link Failed" << std::endl
            << infoLog.data() << std::endl;
        throw std::runtime_error(infoLog.data());
    }
}

void setUniformToProgram(const GLuint& shaderProgram, const std::string& name,
                         const glm::mat4& mat) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(mat));
}

void setUniformToProgram(const GLuint& shaderProgram, const std::string& name,
                         const glm::vec4& vec) {
    glUseProgram(shaderProgram);
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                 glm::value_ptr(vec));
}