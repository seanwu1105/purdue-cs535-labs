#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

const std::vector<glm::vec3> _tessellateIcosahedron(const size_t& divisionCount);
const std::vector<glm::vec3> _subdivideTriangle(
    const std::array<glm::vec3, 3>& triangle,
    const size_t& step
);

class SphereComponent {
public:
    SphereComponent(
        const ShaderProgramProvider& shaderProgramProvider,
        const glm::mat4& projection,
        const glm::vec4& color
    ) : shaderProgram(shaderProgramProvider
                      .getDefaultShaderProgram(typeid(*this))),
        color(color) {
        setUniformToProgram(shaderProgram, "projection", projection);
    }

    void render(const glm::mat4& view, const glm::vec2 location) const {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        setUniformToProgram(shaderProgram, "view", view);

        setUniformToProgram(shaderProgram, "color", color);

        const auto scale{ 0.1f };
        glm::mat4 model(1.f);
        model = glm::scale(model, glm::vec3(scale));
        model = glm::translate(model, { location.x, 1.f, location.y });

        setUniformToProgram(shaderProgram, "model", model);

        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)vertices.size());
    }

private:
    const std::vector<glm::vec3> vertices{ _tessellateIcosahedron(3) };
    mutable GLuint VAO{ buildVAO() };
    mutable GLuint VBO{};
    const GLuint shaderProgram{};
    const glm::vec4 color{};

    const GLuint buildVAO() const {
        if (glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
        if (glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void*)0);

        glEnableVertexAttribArray(0);
        return VAO;
    }
};

const std::vector<glm::vec3> _tessellateIcosahedron(const size_t& divisionCount) {
    const std::array<glm::vec3, 5> baseVertices{ glm::vec3
        {std::cos(0), std::sin(0), 0},
        {std::cos(2.f / 3.f * M_PI), std::sin(2.f / 3.f * M_PI), 0},
        {std::cos(4.f / 3.f * M_PI), std::sin(4.f / 3.f * M_PI), 0},
        {0, 0, 1},
        {0, 0, -1}
    };

    const std::array<std::array<glm::vec3, 3>, 6> triangles{ std::array<glm::vec3, 3>
        {baseVertices[0], baseVertices[1], baseVertices[3]},
        {baseVertices[0], baseVertices[2], baseVertices[3]},
        {baseVertices[1], baseVertices[2], baseVertices[3]},
        {baseVertices[0], baseVertices[1], baseVertices[4]},
        {baseVertices[0], baseVertices[2], baseVertices[4]},
        {baseVertices[1], baseVertices[2], baseVertices[4]},
    };

    std::vector<glm::vec3> vertices{};

    for (const auto& triangle : triangles) {
        const auto divided{ _subdivideTriangle(triangle, divisionCount) };
        vertices.insert(vertices.end(), divided.begin(), divided.end());
    }

    return vertices;
}

const std::vector<glm::vec3> _subdivideTriangle(
    const std::array<glm::vec3, 3>& triangle,
    const size_t& step
) {
    if (step == 0) return { triangle.begin(), triangle.end() };

    const glm::vec3 v01{
        glm::normalize((triangle.at(0) + triangle.at(1)) / 2.f) };
    const glm::vec3 v12{
        glm::normalize((triangle.at(1) + triangle.at(2)) / 2.f) };
    const glm::vec3 v20{
        glm::normalize((triangle.at(2) + triangle.at(0)) / 2.f) };

    std::vector<glm::vec3> vertices{};
    const std::array<glm::vec3, 3> topTriangle{ triangle.at(0), v01, v20 };
    const auto v1{ _subdivideTriangle(topTriangle, step - 1) };
    vertices.insert(vertices.end(), v1.begin(), v1.end());

    const std::array<glm::vec3, 3> leftTriangle{ v01, triangle.at(1), v12 };
    const auto v2{ _subdivideTriangle(leftTriangle, step - 1) };
    vertices.insert(vertices.end(), v2.begin(), v2.end());

    const std::array<glm::vec3, 3> rightTriangle{ v20, v12, triangle.at(2) };
    const auto v3{ _subdivideTriangle(rightTriangle, step - 1) };
    vertices.insert(vertices.end(), v3.begin(), v3.end());

    const std::array<glm::vec3, 3> midTriangle{ v01, v12, v20 };
    const auto v4{ _subdivideTriangle(midTriangle, step - 1) };
    vertices.insert(vertices.end(), v4.begin(), v4.end());

    return vertices;
}