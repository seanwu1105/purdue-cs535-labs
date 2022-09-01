#pragma once

#include <span>
#include <variant>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "result.h"

std::variant<SuccessResult<GLFWwindow* const>, ErrorResult> initGl();

void cleanupGl(GLFWwindow* const window, const GLsizei& size,
               const std::span<GLuint>& shaderPrograms,
               const std::span<GLuint>& VAOs, const std::span<GLuint>& VBOs);

void buildTriangleVertices(const GLuint& VAO, const GLuint& VBO);