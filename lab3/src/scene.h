#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "bullet.h"
#include "components/axes.h"
#include "components/floor.h"
#include "components/grid.h"
#include "components/sphere.h"
#include "player.h"
#include "shader.h"

struct SceneData {
  Player player{};
  Bullet bullet{};
  std::vector<glm::vec2> goodSpherePositions{};
  std::vector<glm::vec2> badSpherePositions{};
};

class Scene {
public:
  void render(const float &viewAspectRatio, const SceneData &data) const {
    // const glm::mat4 view{glm::lookAt(glm::vec3{1, 2, 2}, {}, {0, 1, 0})};
    const glm::mat4 view{glm::lookAt(
        glm::vec3{data.player.position.x, 0.1, data.player.position.y},
        glm::vec3{data.player.position.x +
                      std::cos(glm::radians(data.player.angleDeg)),
                  0.1,
                  data.player.position.y +
                      std::sin(glm::radians(data.player.angleDeg))},
        glm::vec3{0, 1, 0})};

    const glm::mat4 proj{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                          0.0001f, 100.0f)};

    axesComponent.render(view, proj);
    gridComponent.render(view, proj);
    floorComponent.render(view, proj);
    renderGoodSpheres(view, proj, data);
    renderBadSpheres(view, proj, data);
    if (data.bullet.visible)
      bulletSphere.render(view, proj, data.bullet.position);
  }

private:
  mutable SceneData prevData{};

  const AxesComponent axesComponent{};
  const GridComponent gridComponent{};
  const FloorComponent floorComponent{};
  mutable std::vector<SphereComponent> goodSpheres{};
  mutable std::vector<SphereComponent> badSpheres{};
  const SphereComponent bulletSphere{.01f, glm::vec4(0.6f)};

  void renderGoodSpheres(const glm::mat4 &view, const glm::mat4 &proj,
                         const SceneData &data) const {
    if (data.goodSpherePositions != prevData.goodSpherePositions) {
      prevData.goodSpherePositions = data.goodSpherePositions;
      for (const auto &_ : data.goodSpherePositions) {
        const glm::vec4 color{0.f, 0.45, 0.2, 1.0};
        goodSpheres.push_back(SphereComponent{.1f, color});
      }
    }
    for (size_t i = 0; const auto &sphere : goodSpheres) {
      sphere.render(view, proj, data.goodSpherePositions.at(i));
      ++i;
    }
  }

  void renderBadSpheres(const glm::mat4 &view, const glm::mat4 &proj,
                        const SceneData &data) const {
    if (data.badSpherePositions != prevData.badSpherePositions) {
      prevData.badSpherePositions = data.badSpherePositions;
      for (const auto &_ : data.badSpherePositions) {
        const glm::vec4 color{0.45, 0.f, 0.2, 1.0};
        badSpheres.push_back(SphereComponent{.1f, color});
      }
    }
    for (size_t i = 0; const auto &sphere : badSpheres) {
      sphere.render(view, proj, data.badSpherePositions.at(i));
      ++i;
    }
  }
};
