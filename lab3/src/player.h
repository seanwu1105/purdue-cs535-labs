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
  Player newPlayer{player};
  newPlayer.position =
      player.position + glm::vec2{std::cos(glm::radians(player.angleDeg)),
                                  std::sin(glm::radians(player.angleDeg))} *
                            player.speed;
  return newPlayer;
}

Player moveBackward(const Player &player) {
  Player newPlayer{player};
  newPlayer.position =
      player.position - glm::vec2{std::cos(glm::radians(player.angleDeg)),
                                  std::sin(glm::radians(player.angleDeg))} *
                            player.speed;
  return newPlayer;
}

Player turnLeft(const Player &player) {
  Player newPlayer{player};
  newPlayer.angleDeg -= player.speed * 500;
  return newPlayer;
}

Player turnRight(const Player &player) {
  Player newPlayer{player};
  newPlayer.angleDeg += player.speed * 500;
  return newPlayer;
}