#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "scene.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab 3", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    const std::array<glm::vec3, 3> vertices{
            glm::vec3{-0.5f, -0.5f, 0.0f},
            glm::vec3{0.5f, -0.5f, 0.0f},
            glm::vec3{ 0.0f,  0.5f, 0.0f}
    };
    const auto renderObject = buildRenderObject(vertices);
    const auto shaderProgram = createTriangleShaderProgram();

    const std::array<glm::vec3, 3> vertices2{
            glm::vec3{-0.2f, -0.9f, 0.0f},
            glm::vec3{0.3f, -0.6f, 0.0f},
            glm::vec3{ 0.4f,  0.4f, 0.0f}
    };
    const auto renderObject2 = buildRenderObject(vertices2);
    const auto shaderProgram2 = createTriangle2ShaderProgram();

    while (!glfwWindowShouldClose(window)) {
        // TODO: input

        glClear(GL_COLOR_BUFFER_BIT);
        render(renderObject, shaderProgram);
        render(renderObject2, shaderProgram2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}