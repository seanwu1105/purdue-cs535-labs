#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

class TriangleComponent {
public:
    TriangleComponent(const glm::mat4& projection) {
        setUniformToProgram(shaderProgramProvider.program(),
                            "projection", projection);
        setUniformToProgram(shaderProgramProvider.program(), "color",
                            glm::vec4{ 0.6, 0.3, 0.4, 1.0 });
    }

    void render(const glm::mat4& view, const float& data) const {
        if (data != prev_data) {
            buildVAO(data);
            prev_data = data;
        }

        glBindVertexArray(VAO);
        glUseProgram(shaderProgramProvider.program());
        setUniformToProgram(shaderProgramProvider.program(), "view", view);

        const auto rotateOffset{ static_cast<float>(glfwGetTime()) * 100 };
        const auto model{ glm::rotate(glm::mat4(1.0f),
                                      glm::radians(rotateOffset),
                                      glm::vec3(1.0, 1.0, 1.0)) };
        setUniformToProgram(shaderProgramProvider.program(), "model", model);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    static inline const DefaultShaderProgramProvider shaderProgramProvider{};
    mutable float prev_data{};
    mutable GLuint VAO{};
    mutable GLuint VBO{};

    void buildVAO(const float& k) const {
        if (glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
        if (glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        const std::array<glm::vec3, 3> vertices{
            glm::vec3{-k, -k, 0.0f},
            glm::vec3{k, -k, 0.0f},
            glm::vec3{ 0.0f,  k, 0.0f}
        };
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              static_cast<void*>(0));

        glEnableVertexAttribArray(0);
    }
};