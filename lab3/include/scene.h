#pragma once

#include <array>

#include "glad/glad.h"
#include "glm/vec3.hpp"

struct RenderObject {
    const GLuint VAO;
    const GLuint VBO; // May be unnecessary.
};

void render(const RenderObject& renderObject, const GLuint shaderProgram) {
    glBindVertexArray(renderObject.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderObject.VBO); // May be unnecessary.
    glUseProgram(shaderProgram);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

const RenderObject buildRenderObject(const std::array<glm::vec3, 3>& vertices) {
    // TODO: Use glIsBuffer to check if we need to delete the VAO and VBO
    // TODO: Move buildScene into render

    GLuint VAO{};
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO{};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    return { VAO, VBO };
}

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
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
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
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const auto fragmentShaderSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "  FragColor = vec4(1.0f, 0.5f, 0.5f, 1.0f);\n"
        "}";

    return createShaderProgram(&vertexShaderSrc, &fragmentShaderSrc);
}