#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "utils.h"

class FloorComponent {
private:
    const std::array<glm::vec3, 4> vertices{
            glm::vec3{-1.0, -1.0, 0.0},
            glm::vec3{-1.0, 1.0, 0.0},
            glm::vec3{1.0, -1.0, 0.0},
            glm::vec3{1.0, 1.0, 0.0},
    };
    mutable GLuint VAO{ buildVAO() };
    mutable GLuint VBO{};
    const GLuint shaderProgram{ buildShaderProgram({
        {"floor.vert", GL_VERTEX_SHADER},
        {"floor.frag", GL_FRAGMENT_SHADER} }) };

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

public:
    FloorComponent() {
        glUseProgram(shaderProgram);

        auto model{ glm::mat4(1.0) };
        auto view{ glm::mat4(1.0) };
        auto projection{ glm::mat4(1.0) };

        model = glm::rotate(model, glm::radians(-90.0f),
                            glm::vec3{ 1.0, 0.0, 0.0 });
        view = glm::translate(view, glm::vec3{ 0.0f, -0.5f, -2.0f });
        projection = glm::perspective(glm::radians(45.0f),
                                      getViewAspectRatio(), 0.0f, 100.0f);

        setUniformToProgram(shaderProgram, "model", model);
        setUniformToProgram(shaderProgram, "view", view);
        setUniformToProgram(shaderProgram, "projection", projection);
    }

    void render() const {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size());
    }
};