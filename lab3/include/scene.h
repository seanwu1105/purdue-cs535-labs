#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "components/triangle.h"
#include "components/floor.h"

class Scene {
private:
    const TriangleComponent triangleComponent1{ {
        {"triangle.vert", GL_VERTEX_SHADER},
        {"triangle.frag", GL_FRAGMENT_SHADER} } };
    const TriangleComponent triangleComponent2{ {
        {"triangle.vert", GL_VERTEX_SHADER},
        {"triangle2.frag", GL_FRAGMENT_SHADER} } };
    const FloorComponent floorComponent{};

public:
    void render(const float& data, const float& data2) const {
        //triangleComponent1.render(data);
        //triangleComponent2.render(data2);
        floorComponent.render();
    }
};
