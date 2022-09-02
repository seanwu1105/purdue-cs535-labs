#pragma once

#include <span>
#include <variant>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "result.h"

struct RenderObject {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint VBO;
};

const std::variant<SuccessResult<GLFWwindow* const>, ErrorResult> initGl();

void cleanupGl(GLFWwindow* const window, const GLsizei& size,
               const std::span<const GLuint>& shaderPrograms,
               const std::span<GLuint>& VAOs,
               const std::span<GLuint>& VBOs);

void buildTriangleVertices(const GLuint& VAO, const GLuint& VBO);

const std::variant<SuccessResult<const GLuint>, ErrorResult> attachShader(
    const GLuint& shaderProgram,
    const std::string& shaderFilename,
    const GLenum& shaderType
);