#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "components/axes.h"
#include "components/grid.h"
#include "components/triangle.h"
#include "components/floor.h"
#include "components/sphere.h"

struct SceneData {
    std::vector<glm::vec2> goodSphereLocations;
    std::vector<glm::vec2> badSphereLocations;
};

class Scene {
public:
    void render(const SceneData& data) const {
        axesComponent.render(view);
        gridComponent.render(view);
        triangleComponent.render(view, 0.5f);
        floorComponent.render(view);
        renderGoodSpheres(data);
        renderBadSpheres(data);
    }

private:
    const glm::mat4 projection{ glm::perspective(glm::radians(45.0f),
                                                  getViewAspectRatio(),
                                                  0.1f, 100.0f) };
    const glm::mat4 view{ glm::lookAt(glm::vec3{ -1, 2, 2 }, glm::vec3{},
                                      glm::vec3{ 0, 1, 0 }) };

    mutable SceneData preData{};

    const AxesComponent axesComponent{ projection };
    const GridComponent gridComponent{ projection };
    const TriangleComponent triangleComponent{ projection };
    const FloorComponent floorComponent{ projection };
    mutable std::vector<SphereComponent> goodSpheres{};
    mutable std::vector<SphereComponent> badSpheres{};

    void renderGoodSpheres(const SceneData& data) const {
        if (data.goodSphereLocations != preData.goodSphereLocations) {
            preData.goodSphereLocations = data.goodSphereLocations;
            for (const auto& _ : data.goodSphereLocations) {
                const glm::vec4 color{ 0.f, 0.45, 0.2, 1.0 };
                goodSpheres.push_back(SphereComponent{ projection, color });
            }
        }
        for (size_t i = 0; const auto & sphere : goodSpheres) {
            sphere.render(view, data.goodSphereLocations.at(i));
            ++i;
        }
    }

    void renderBadSpheres(const SceneData& data) const {
        if (data.badSphereLocations != preData.badSphereLocations) {
            preData.badSphereLocations = data.badSphereLocations;
            for (const auto& _ : data.badSphereLocations) {
                const glm::vec4 color{ 0.45, 0.f, 0.2, 1.0 };
                badSpheres.push_back(SphereComponent{ projection, color });
            }
        }
        for (size_t i = 0; const auto & sphere : badSpheres) {
            sphere.render(view, data.badSphereLocations.at(i));
            ++i;
        }
    }
};
