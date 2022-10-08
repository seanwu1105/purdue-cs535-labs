#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

class TriangleVaoProvider {
public:
  const GLuint &vao() const {
    if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const auto k{0.5f};
    const std::array<glm::vec3, 3> vertices{glm::vec3{-k, -k, 0.0f},
                                            glm::vec3{k, -k, 0.0f},
                                            glm::vec3{0.0f, k, 0.0f}};
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(0));

    glEnableVertexAttribArray(0);

    return _vao;
  }

private:
  mutable GLuint _vao{};
};

class TriangleComponent {
public:
  TriangleComponent() {
    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{0.6, 0.3, 0.4, 1.0});
  }

  void render(const glm::mat4 &view, const glm::mat4 &proj) const {
    glBindVertexArray(vaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());
    setUniformToProgram(shaderProgramProvider.program(), "view", view);
    setUniformToProgram(shaderProgramProvider.program(), "proj", proj);

    const auto rotateOffset{static_cast<float>(glfwGetTime()) * 100};
    const auto model{glm::rotate(glm::mat4(1.0f), glm::radians(rotateOffset),
                                 glm::vec3(1.0, 1.0, 1.0))};
    setUniformToProgram(shaderProgramProvider.program(), "model", model);

    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

private:
  static inline const DefaultShaderProgramProvider shaderProgramProvider{};
  static inline const TriangleVaoProvider vaoProvider{};
};