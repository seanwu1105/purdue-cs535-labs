#pragma once

#include <array>

#include "glm/vec3.hpp"

void buildScene() {
    const std::array<glm::vec3, 3> vertices{
        glm::vec3{-0.5f, -0.5f, 0.0f},
        glm::vec3{0.5f, -0.5f, 0.0f},
        glm::vec3{ 0.0f,  0.5f, 0.0f}
    };
}

void render() {
    buildScene();
}