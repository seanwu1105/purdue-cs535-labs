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
  std::vector<SphereData> goodSpheres{};
  std::vector<SphereData> badSpheres{};
};

class Scene {
public:
  void render(const float &viewAspectRatio, const SceneData &data) const {
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
      bulletSphere.render(view, proj,
                          {.position{data.bullet.position},
                           .scale{0.01f},
                           .color{glm::vec4(0.6f)}});
  }

private:
  mutable SceneData prevData{};

  const AxesComponent axesComponent{};
  const GridComponent gridComponent{};
  const FloorComponent floorComponent{};
  mutable std::vector<SphereComponent> goodSpheres{};
  mutable std::vector<SphereComponent> badSpheres{};
  const SphereComponent bulletSphere{};

  void renderGoodSpheres(const glm::mat4 &view, const glm::mat4 &proj,
                         const SceneData &data) const {
    if (data.goodSpheres.size() != prevData.goodSpheres.size()) {
      prevData.goodSpheres = data.goodSpheres;
      goodSpheres.clear();
      for (const auto &_ : data.goodSpheres)
        goodSpheres.push_back(SphereComponent{});
    }

    for (size_t i = 0; const auto &sphere : goodSpheres) {
      sphere.render(view, proj, data.goodSpheres.at(i));
      ++i;
    }
  }

  void renderBadSpheres(const glm::mat4 &view, const glm::mat4 &proj,
                        const SceneData &data) const {
    if (data.badSpheres.size() != prevData.badSpheres.size()) {
      prevData.badSpheres = data.badSpheres;
      badSpheres.clear();
      for (const auto &_ : data.badSpheres)
        badSpheres.push_back(SphereComponent{});
    }
    for (size_t i = 0; const auto &sphere : badSpheres) {
      sphere.render(view, proj, data.badSpheres.at(i));
      ++i;
    }
  }
};
