#pragma once

#include <cmath>

#include <glm/gtx/scalar_multiplication.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>

struct Player {
  glm::vec2 position{};
  float angleDeg{};
  float speed{0.00025f};
};

Player moveForward(const Player &player) {
  return Player{.position{player.position +
                          glm::vec2{std::cos(glm::radians(player.angleDeg)),
                                    std::sin(glm::radians(player.angleDeg))} *
                              player.speed},
                .angleDeg{player.angleDeg},
                .speed{player.speed}};
}

Player moveBackward(const Player &player) {
  return Player{.position{player.position -
                          glm::vec2{std::cos(glm::radians(player.angleDeg)),
                                    std::sin(glm::radians(player.angleDeg))} *
                              player.speed},
                .angleDeg{player.angleDeg},
                .speed{player.speed}};
}