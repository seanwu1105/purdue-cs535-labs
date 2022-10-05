// TODO: Draw arrow

#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "../utils.h"

class AxesComponent {
public:
    AxesComponent(
        const ShaderProgramProvider& shaderProgramProvider,
        const glm::mat4& projection
    ) : shaderProgram(shaderProgramProvider
                      .getDefaultShaderProgram(typeid(*this))) {
        setUniformToProgram(shaderProgram, "model", glm::mat4(1.0));
        setUniformToProgram(shaderProgram, "projection", projection);
    }

    void render(const glm::mat4& view) const {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        setUniformToProgram(shaderProgram, "view", view);

        setUniformToProgram(shaderProgram, "color",
                            glm::vec4{ 1.0, 0.0, 0.0, 1.0 });
        glDrawArrays(GL_LINES, 0, 2);

        setUniformToProgram(shaderProgram, "color",
                            glm::vec4{ 0.0, 1.0, 0.0, 1.0 });
        glDrawArrays(GL_LINES, 2, 2);

        setUniformToProgram(shaderProgram, "color",
                            glm::vec4{ 0.0, 0.0, 1.0, 1.0 });
        glDrawArrays(GL_LINES, 4, 2);
    }

private:
    const std::array<glm::vec3, 6> vertices{
        glm::vec3{-1, 0, 0},
        glm::vec3{1, 0, 0},
        glm::vec3{0, -1, 0},
        glm::vec3{0, 1, 0},
        glm::vec3{0, 0, -1},
        glm::vec3{0, 0, 1}
    };
    mutable GLuint VAO{ buildVAO() };
    mutable GLuint VBO{};
    const GLuint shaderProgram{};

    const GLuint buildVAO() const noexcept {
        if (glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
        if (glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              static_cast<void*>(0));

        glEnableVertexAttribArray(0);

        return VAO;
    }
};