#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

const std::vector<glm::vec3> _tessellateIcosahedron(
    const size_t& divisionCount
);
const std::vector<glm::vec3> _subdivideTriangle(
    const std::array<glm::vec3, 3>& triangle,
    const size_t& step
);

class SphereVaoProvider {
public:
    const std::vector<glm::vec3> vertices{ _tessellateIcosahedron(3) };

    GLuint vao() const {
        if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              static_cast<void*>(0));

        glEnableVertexAttribArray(0);
        return _vao;
    }

private:
    mutable GLuint _vao{};
};

class SphereComponent {
public:
    SphereComponent(
        const glm::mat4& projection,
        const glm::vec4& color
    ) : color(color) {
        setUniformToProgram(shaderProgramProvider.program(),
                            "projection", projection);
    }

    void render(const glm::mat4& view, const glm::vec2 location) const {
        glBindVertexArray(vaoProvider.vao());
        glUseProgram(shaderProgramProvider.program());
        setUniformToProgram(shaderProgramProvider.program(), "view", view);

        setUniformToProgram(shaderProgramProvider.program(), "color", color);

        constexpr auto scale{ 0.1f };
        glm::mat4 model(1.f);
        model = glm::scale(model, glm::vec3(scale));
        model = glm::translate(model, { location.x, 1.f, location.y });

        setUniformToProgram(shaderProgramProvider.program(), "model", model);

        glDrawArrays(GL_LINE_LOOP, 0,
                     static_cast<GLsizei>(vaoProvider.vertices.size()));
    }

private:
    static inline const DefaultShaderProgramProvider shaderProgramProvider{};
    static inline const SphereVaoProvider vaoProvider{};
    const glm::vec4 color{};
};

const std::vector<glm::vec3> _tessellateIcosahedron(
    const size_t& divisionCount
) {
    const std::array<glm::vec3, 5> baseVertices{ glm::vec3
        {std::cos(0), std::sin(0), 0},
        {std::cos(2.f / 3.f * M_PI), std::sin(2.f / 3.f * M_PI), 0},
        {std::cos(4.f / 3.f * M_PI), std::sin(4.f / 3.f * M_PI), 0},
        {0, 0, 1},
        {0, 0, -1}
    };

    const std::array<std::array<glm::vec3, 3>, 6> triangles{ {
        {baseVertices.at(0), baseVertices.at(1), baseVertices.at(3)},
        {baseVertices.at(0), baseVertices.at(2), baseVertices.at(3)},
        {baseVertices.at(1), baseVertices.at(2), baseVertices.at(3)},
        {baseVertices.at(0), baseVertices.at(1), baseVertices.at(4)},
        {baseVertices.at(0), baseVertices.at(2), baseVertices.at(4)},
        {baseVertices.at(1), baseVertices.at(2), baseVertices.at(4)},
    } };

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