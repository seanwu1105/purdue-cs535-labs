#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "components/axes.h"
#include "components/triangle.h"
#include "components/floor.h"

class Scene {
private:
    const glm::mat4 projection{ glm::perspective(glm::radians(45.0f),
                                                  getViewAspectRatio(),
                                                  0.1f, 100.0f) };

    const AxesComponent axesComponent{ projection };
    const TriangleComponent triangleComponent{ projection };
    const FloorComponent floorComponent{ projection };

public:
    void render(const float& data, const float& data2) const {
        axesComponent.render();
        triangleComponent.render(data);
        floorComponent.render();
    }
};
