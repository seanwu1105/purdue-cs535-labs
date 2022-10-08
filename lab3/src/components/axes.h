// TODO: Draw arrow

#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "../utils.h"

class AxesVaoProvider {
public:
  const GLuint &vao() const {
    if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const std::array<glm::vec3, 6> vertices{
        {{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}}};

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

class AxesComponent {
public:
  AxesComponent() {
    setUniformToProgram(shaderProgramProvider.program(), "model",
                        glm::mat4(1.0));
  }

  void render(const glm::mat4 &view, const glm::mat4 &proj) const {
    glBindVertexArray(VaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());
    setUniformToProgram(shaderProgramProvider.program(), "view", view);
    setUniformToProgram(shaderProgramProvider.program(), "proj", proj);

    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{1.0, 0.0, 0.0, 1.0});
    glDrawArrays(GL_LINES, 0, 2);

    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{0.0, 1.0, 0.0, 1.0});
    glDrawArrays(GL_LINES, 2, 2);

    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{0.0, 0.0, 1.0, 1.0});
    glDrawArrays(GL_LINES, 4, 2);
  }

private:
  static inline const DefaultShaderProgramProvider shaderProgramProvider{};
  static inline const AxesVaoProvider VaoProvider{};
};
