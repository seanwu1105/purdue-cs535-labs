#pragma once

#include <array>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"

class TriangleComponent {
private:
    mutable GLuint VAO{};
    mutable GLuint VBO{};
    mutable std::array<glm::vec3, 3> prev_vertices{};

    void buildVAO(const std::array<glm::vec3, 3>& vertices) const {
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
    }

public:
    void render(const std::array<glm::vec3, 3>& vertices,
                const GLuint shaderProgram) const {
        if (vertices != prev_vertices) {
            buildVAO(vertices);
            prev_vertices = vertices;
        }

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);

        const auto rotateOffset{ (float)glfwGetTime() * 100 };
        const auto trans{ glm::rotate(glm::mat4(1.0f),
                                      glm::radians(rotateOffset),
                                      glm::vec3(0.0, 1.0, 1.0)) };
        const auto transLoc{ glGetUniformLocation(shaderProgram, "trans") };
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};