#include "glm/glm.hpp"

#include "gl.h"

std::variant<SuccessResult<GLFWwindow* const>, ErrorResult> initGl() {
    if (!glfwInit()) return ErrorResult{ "Fail to init glfw." };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto window{ glfwCreateWindow(800, 800, "Simple",
                                        nullptr, nullptr) };
    if (window == nullptr) {
        glfwTerminate();
        return ErrorResult{ "Cannot open GLFW window." };
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();
    return SuccessResult<GLFWwindow* const>{ window };
}

void cleanupGl(GLFWwindow* const window,
               const GLsizei& size,
               const std::span<GLuint>& shaderPrograms,
               const std::span<GLuint>& VAOs, const std::span<GLuint>& VBOs) {
    for (const auto& program : shaderPrograms)
        glDeleteProgram(program);

    glDeleteVertexArrays(size, VAOs.data());
    glDeleteBuffers(size, VBOs.data());

    glfwDestroyWindow(window);
    glfwTerminate();
}

void buildTriangleVertices(const GLuint& VAO, const GLuint& VBO) {
    const glm::mediump_float k{ 0.5f };

    // TODO: Use std::array
    glm::vec3 vertices[]
    {
        glm::vec3{ -k, -k, 0.0f },
        glm::vec3{ k, -k, 0.0f },
        glm::vec3{ 0,  k, 0.0f }
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // TODO: Use array.size()
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(glm::mediump_float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
