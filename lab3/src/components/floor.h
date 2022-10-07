#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "../utils.h"

const std::vector<glm::vec3> _tessellateFloor(const GLsizei &divisionCount);

class FloorVaoProvider {
public:
  const GLsizei divisionCount{20};

  GLuint vao() const {
    if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const auto vertices = _tessellateFloor(divisionCount);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(0));

    glEnableVertexAttribArray(0);

    return _vao;
  };

private:
  mutable GLuint _vao{};
};

class FloorComponent {
public:
  FloorComponent(const glm::mat4 &projection) {
    auto model{glm::mat4(1.0)};

    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3{1.0, 0.0, 0.0});

    setUniformToProgram(shaderProgramProvider.program(), "model", model);
    setUniformToProgram(shaderProgramProvider.program(), "projection",
                        projection);
    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4(0.7f));
  }

  void render(const glm::mat4 &view) const {
    glBindVertexArray(vaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());
    setUniformToProgram(shaderProgramProvider.program(), "view", view);

    glDrawArrays(GL_TRIANGLE_STRIP, 0,
                 vaoProvider.divisionCount * vaoProvider.divisionCount * 6);
  }

private:
  static inline const DefaultShaderProgramProvider shaderProgramProvider{};
  static inline const FloorVaoProvider vaoProvider{};
};

const std::vector<glm::vec3> _tessellateFloor(const GLsizei &divisionCount) {
  constexpr auto rangeMin{-1.0f};
  constexpr auto rangeMax{1.0f};
  const auto divisionSize{std::abs(rangeMax - rangeMin) / divisionCount};
  std::vector<glm::vec3> vertices{};
  for (size_t i = 0; i < divisionCount; i += 2) {
    for (size_t j = 0; j < divisionCount; j++) {
      vertices.push_back(
          {rangeMin + i * divisionSize, rangeMin + j * divisionSize, 0.0});
      vertices.push_back({rangeMin + (i + 1) * divisionSize,
                          rangeMin + j * divisionSize, 0.0});
      vertices.push_back({rangeMin + (i + 1) * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
      vertices.push_back(
          {rangeMin + i * divisionSize, rangeMin + j * divisionSize, 0.0});
      vertices.push_back({rangeMin + (i + 1) * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
      vertices.push_back({rangeMin + i * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
    }

    const auto k = i + 1;

    for (int j = divisionCount - 1; j >= 0; j--) {
      vertices.push_back(
          {rangeMin + k * divisionSize, rangeMin + j * divisionSize, 0.0});
      vertices.push_back({rangeMin + k * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
      vertices.push_back({rangeMin + (k + 1) * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
      vertices.push_back(
          {rangeMin + k * divisionSize, rangeMin + j * divisionSize, 0.0});
      vertices.push_back({rangeMin + (k + 1) * divisionSize,
                          rangeMin + (j + 1) * divisionSize, 0.0});
      vertices.push_back({rangeMin + (k + 1) * divisionSize,
                          rangeMin + j * divisionSize, 0.0});
    }
  }

  return vertices;
}