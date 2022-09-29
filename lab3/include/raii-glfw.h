#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class RaiiGlfw {
public:
    RaiiGlfw() {
        glfwSetErrorCallback([](int code, const char* description) {
            std::cout << "GLFW error: [" << code << "] " << description
                << std::endl;
            throw std::runtime_error(description);
                             });

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    ~RaiiGlfw() {
        glfwTerminate();
    }
};