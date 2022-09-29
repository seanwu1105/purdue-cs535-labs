#pragma once

#include <array>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "components/triangle.h"

class Scene {
private:
    const TriangleComponent triangleComponent1{};
    const TriangleComponent triangleComponent2{};

public:
    void render(
        const float& data,
        const GLuint shaderProgram,
        const float& data2,
        const GLuint shaderProgram2
    ) const {
        triangleComponent1.render(data, shaderProgram);
        triangleComponent2.render(data2, shaderProgram2);
    }
};
