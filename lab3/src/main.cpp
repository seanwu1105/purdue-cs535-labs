#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "raii-glfw.h"
#include "scene.h"
#include "shader.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    const RaiiGlfw raiiGlfw{};

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab 3", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    const float k{ 0.5f };
    const float k2{ 0.3f };

    const Scene scene{};

    while (!glfwWindowShouldClose(window)) {
        // TODO: input

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render(k, k2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}