#pragma once

#include <cmath>

#include <glm/gtx/scalar_multiplication.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>

struct Bullet {
  bool visible{false};
  glm::vec2 position{};
  float angleDeg{};
  float speed{0.001f};
};

const Bullet moveBullet(const Bullet &bullet) {
  Bullet newBullet{bullet};
  newBullet.position =
      bullet.position + glm::vec2{std::cos(glm::radians(bullet.angleDeg)),
                                  std::sin(glm::radians(bullet.angleDeg))} *
                            bullet.speed;
  return newBullet;
}