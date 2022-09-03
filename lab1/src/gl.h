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

void cleanupGl(
    GLFWwindow* const window,
    const std::span<const RenderObject>& renderObjects
);

void buildTriangleVertices(const RenderObject& renderObject,
                           const std::array<const glm::vec3, 3>& vertices);

const std::variant<SuccessResult<const GLuint>, ErrorResult> attachShader(
    const GLuint& shaderProgram,
    const std::string& shaderFilename,
    const GLenum& shaderType
);