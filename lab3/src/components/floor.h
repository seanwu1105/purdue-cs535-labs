#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "../utils.h"

const std::vector<glm::vec3> tessellateFloor(const GLsizei& divisionCount);

class FloorComponent {
private:
    const GLsizei divisionCount{ 20 };
    mutable GLuint VAO{ buildVAO() };
    mutable GLuint VBO{};
    const GLuint shaderProgram{ buildShaderProgram({
        {"default.vert", GL_VERTEX_SHADER},
        {"default.frag", GL_FRAGMENT_SHADER} }) };

    const GLuint buildVAO() const {
        if (glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
        if (glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        const auto vertices = tessellateFloor(divisionCount);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void*)0);

        glEnableVertexAttribArray(0);

        return VAO;
    }

public:
    FloorComponent(const glm::mat4& projection) {
        auto model{ glm::mat4(1.0) };

        model = glm::rotate(model, glm::radians(-90.0f),
                            glm::vec3{ 1.0, 0.0, 0.0 });

        setUniformToProgram(shaderProgram, "model", model);
        setUniformToProgram(shaderProgram, "projection", projection);
        setUniformToProgram(shaderProgram, "color", glm::vec4(0.7f));
    }

    void render(const glm::mat4& view) const {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        setUniformToProgram(shaderProgram, "view", view);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, divisionCount * divisionCount * 6);
    }
};

const std::vector<glm::vec3> tessellateFloor(const GLsizei& divisionCount) {
    const auto rangeMin{ -1.0f };
    const auto rangeMax{ 1.0f };
    const auto divisionSize{ std::abs(rangeMax - rangeMin) / divisionCount };
    std::vector<glm::vec3> vertices{};
    for (size_t i = 0; i < divisionCount; i+=2) {
        for (size_t j = 0; j < divisionCount; j++) {
            vertices.push_back({ rangeMin + i * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (i + 1) * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (i + 1) * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + i * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (i + 1) * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + i * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
        }
        
        const auto k = i + 1;

        for (int j = divisionCount - 1; j >= 0; j--) {
            vertices.push_back({ rangeMin + k * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + k * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (k + 1) * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + k * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (k + 1) * divisionSize,
                                 rangeMin + (j + 1) * divisionSize,
                                 0.0 });
            vertices.push_back({ rangeMin + (k + 1) * divisionSize,
                                 rangeMin + j * divisionSize,
                                 0.0 });
        }
    }

    return vertices;
}