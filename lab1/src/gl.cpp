#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"

#include "gl.h"

const std::variant<SuccessResult<const std::string>, ErrorResult> readFile(
    const std::string& filename
);

const std::variant<SuccessResult<GLFWwindow* const>, ErrorResult> initGl() {
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
    const GLint size{ 3 };
    const glm::mediump_float k{ 0.5f };

    const std::array<const glm::vec3, size> vertices{
        glm::vec3{ -k, -k, 0.0f },
        glm::vec3{ k, -k, 0.0f },
        glm::vec3{ 0,  k, 0.0f },
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

const std::variant<SuccessResult<const GLuint>, ErrorResult> attachShader(
    const GLuint& shaderProgram,
    const std::string& shaderFilename,
    const GLenum& shaderType
) {
    const auto readFileResult{ readFile(shaderFilename) };
    if (std::holds_alternative<ErrorResult>(readFileResult))
        return std::get<ErrorResult>(readFileResult);

    const auto shaderSrc{
        std::get<SuccessResult<const std::string>>(readFileResult).value.c_str()
    };

    const auto shader{ glCreateShader(shaderType) };
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    glAttachShader(shaderProgram, shader);
    glLinkProgram(shaderProgram);

    glDeleteShader(shader);

    return SuccessResult<const GLuint>{shaderProgram};
}

const std::variant<SuccessResult<const std::string>, ErrorResult> readFile(
    const std::string& filename
) {
    const auto sourcePath = std::filesystem::path("src");
    std::ifstream shaderFile(sourcePath / filename);
    if (!shaderFile.is_open()) return ErrorResult{
        "Fail to open file: " + (sourcePath / filename).string()
    };

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return SuccessResult<const std::string>{ buffer.str() };
}