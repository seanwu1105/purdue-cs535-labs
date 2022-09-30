#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class TriangleComponent {
private:
    mutable float prev_data{};
    mutable GLuint VAO{};
    mutable GLuint VBO{};
    const GLuint shaderProgram{ buildShaderProgram({
        {"default.vert", GL_VERTEX_SHADER},
        {"triangle.frag", GL_FRAGMENT_SHADER} }) };

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
                              (void*)0);

        glEnableVertexAttribArray(0);
    }

public:
    TriangleComponent(const glm::mat4 projection) {
        auto view{ glm::mat4(1.0) };

        view = glm::translate(view, glm::vec3{ 0.0, -0.5, -3.0 });
        setUniformToProgram(shaderProgram, "view", view);
        setUniformToProgram(shaderProgram, "projection", projection);
    }

    void render(const float& data) const {
        if (data != prev_data) {
            buildVAO(data);
            prev_data = data;
        }

        glBindVertexArray(VAO);

        const auto rotateOffset{ (float)glfwGetTime() * 100 };
        const auto model{ glm::rotate(glm::mat4(1.0f),
                                      glm::radians(rotateOffset),
                                      glm::vec3(0.0, 1.0, 1.0)) };
        setUniformToProgram(shaderProgram, "model", model);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};