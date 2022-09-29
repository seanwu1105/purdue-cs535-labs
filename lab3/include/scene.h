#pragma once

#include <array>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "components/triangle.h"

class Scene {
private:
    const TriangleComponent triangleComponent1{};
    const TriangleComponent triangleComponent2{};

public:
    void render(const std::array<glm::vec3, 3>& vertices,
                const GLuint shaderProgram,
                const std::array<glm::vec3, 3>& vertices2,
                const GLuint shaderProgram2) const {
        triangleComponent1.render(vertices, shaderProgram);
        triangleComponent2.render(vertices2, shaderProgram2);
    }
};

const GLuint createShaderProgram(const GLchar* const* vertexShaderSrc, const GLchar* const* fragmentShaderSrc) {
    // TODO: Use memorization.
    // TODO: Check compilation status.
    const auto shaderProgram{ glCreateProgram() };

    const std::array<GLuint, 2> shaders{
        glCreateShader(GL_VERTEX_SHADER),
        glCreateShader(GL_FRAGMENT_SHADER)
    };
    glShaderSource(shaders.at(0), 1, vertexShaderSrc, nullptr);
    glShaderSource(shaders.at(1), 1, fragmentShaderSrc, nullptr);
    for (const auto& shader : shaders) {
        glCompileShader(shader);
        glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }
    glLinkProgram(shaderProgram);

    return shaderProgram;
}

const auto createTriangleShaderProgram() {
    const auto vertexShaderSrc = "#version 330 core\n"
        "layout (location = 0) in vec3 iPosition;\n"
        "uniform mat4 trans;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = trans * vec4(iPosition.xyz, 1.0);\n"
        "}\0";

    const auto fragmentShaderSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";

    return createShaderProgram(&vertexShaderSrc, &fragmentShaderSrc);
}

const auto createTriangle2ShaderProgram() {
    const auto vertexShaderSrc = "#version 330 core\n"
        "layout (location = 0) in vec3 iPosition;\n"
        "uniform mat4 trans;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = trans * vec4(iPosition.xyz, 1.0);\n"
        "}\0";


    const auto fragmentShaderSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "  FragColor = vec4(1.0f, 0.5f, 0.5f, 1.0f);\n"
        "}";

    return createShaderProgram(&vertexShaderSrc, &fragmentShaderSrc);
}