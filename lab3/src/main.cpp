// TODO: Reuse shader programs
// TODO: Reuse VAOs

#define _USE_MATH_DEFINES

#include <cmath>
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

    const Scene scene{};

    const SceneData data{
        .goodSphereLocations{
            { -5.f, -5.f },
            { 2.f, -3.f },
            { -6.f, 4.f },
            { 7.f, 9.f },
            { 1.f, 0.f }
        },
        .badSphereLocations{
            {-7.f, -3.f},
            { 3.f, 6.f },
            { -5.f, 7.f },
            { 8.f, -1.f },
            { 2.f, -7.f }
        }
    };

    while (!glfwWindowShouldClose(window)) {
        // TODO: input

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render(data);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}