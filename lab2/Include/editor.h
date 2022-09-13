#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

GLFWwindow* createEditorWindow() {
    if (glfwInit() != GLFW_TRUE) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "XY Plane", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) return nullptr;
    glViewport(0, 0, 800, 800);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    return window;
}

int renderEditor(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    return 0;
}