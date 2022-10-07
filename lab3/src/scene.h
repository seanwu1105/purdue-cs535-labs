#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "components/axes.h"
#include "components/floor.h"
#include "components/grid.h"
#include "components/sphere.h"
#include "components/triangle.h"
#include "shader.h"

struct SceneData {
  std::vector<glm::vec2> goodSphereLocations;
  std::vector<glm::vec2> badSphereLocations;
};

class Scene {
public:
  void render(const float &viewAspectRatio, const SceneData &data) const {
    const glm::mat4 proj{
        glm::perspective(glm::radians(45.0f), viewAspectRatio, 0.1f, 100.0f)};

    axesComponent.render(view, proj);
    gridComponent.render(view, proj);
    triangleComponent.render(view, proj);
    floorComponent.render(view, proj);
    renderGoodSpheres(proj, data);
    renderBadSpheres(proj, data);
  }

private:
  const glm::mat4 view{
      glm::lookAt(glm::vec3{-1, 2, 2}, glm::vec3{}, glm::vec3{0, 1, 0})};

  mutable SceneData preData{};

  const AxesComponent axesComponent{};
  const GridComponent gridComponent{};
  const TriangleComponent triangleComponent{};
  const FloorComponent floorComponent{};
  mutable std::vector<SphereComponent> goodSpheres{};
  mutable std::vector<SphereComponent> badSpheres{};

  void renderGoodSpheres(const glm::mat4 &proj, const SceneData &data) const {
    if (data.goodSphereLocations != preData.goodSphereLocations) {
      preData.goodSphereLocations = data.goodSphereLocations;
      for (const auto &_ : data.goodSphereLocations) {
        const glm::vec4 color{0.f, 0.45, 0.2, 1.0};
        goodSpheres.push_back(SphereComponent{color});
      }
    }
    for (size_t i = 0; const auto &sphere : goodSpheres) {
      sphere.render(view, proj, data.goodSphereLocations.at(i));
      ++i;
    }
  }

  void renderBadSpheres(const glm::mat4 &proj, const SceneData &data) const {
    if (data.badSphereLocations != preData.badSphereLocations) {
      preData.badSphereLocations = data.badSphereLocations;
      for (const auto &_ : data.badSphereLocations) {
        const glm::vec4 color{0.45, 0.f, 0.2, 1.0};
        badSpheres.push_back(SphereComponent{color});
      }
    }
    for (size_t i = 0; const auto &sphere : badSpheres) {
      sphere.render(view, proj, data.badSphereLocations.at(i));
      ++i;
    }
  }
};
