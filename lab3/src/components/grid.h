#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "../utils.h"

class GridComponent {
public:
    GridComponent(
        const ShaderProgramProvider& shaderProgramProvider,
        const glm::mat4& projection
    ) : shaderProgram(shaderProgramProvider
                      .getDefaultShaderProgram(typeid(*this))) {
        auto model{ glm::mat4(1.0) };

        model = glm::rotate(model, glm::radians(-90.0f),
                            glm::vec3{ 1.0, 0.0, 0.0 });


        setUniformToProgram(shaderProgram, "model", model);
        setUniformToProgram(shaderProgram, "projection", projection);
        setUniformToProgram(shaderProgram, "color", glm::vec4(0.6f));
    }

    void render(const glm::mat4& view) const {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        setUniformToProgram(shaderProgram, "view", view);

        glDrawArrays(GL_LINES, 0, (gridCount + 1) * 2);
        glDrawArrays(GL_LINES, (gridCount + 1) * 2, (gridCount + 1) * 2);
    }

private:
    const GLsizei gridCount{ 20 };
    mutable GLuint VAO{ buildVAO() };
    mutable GLuint VBO{};
    const GLuint shaderProgram{};

    const GLuint buildVAO() const {
        if (glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
        if (glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        const auto gridMin{ -1.0f };
        const auto gridMax{ 1.0f };
        const auto divisionSize{ std::abs(gridMax - gridMin) / gridCount };
        std::vector<glm::vec3> vertices{};
        for (size_t i = 0; i <= gridCount; i++) {
            vertices.push_back({ gridMin + divisionSize * i, gridMin, 0.0f });
            vertices.push_back({ gridMin + divisionSize * i, gridMax, 0.0f });
        }
        for (size_t i = 0; i <= gridCount; i++) {
            vertices.push_back({ gridMin, gridMin + divisionSize * i, 0.0f });
            vertices.push_back({ gridMax, gridMin + divisionSize * i, 0.0f });
        }

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void*)0);

        glEnableVertexAttribArray(0);

        return VAO;
    }
};