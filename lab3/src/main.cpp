#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "bullet.h"
#include "player.h"
#include "raii-glfw.h"
#include "scene.h"
#include "shader.h"
#include "utils.h"

void framebufferSizeCallback(GLFWwindow *window, int width,
                             int height) noexcept;
const Player updatePlayer(GLFWwindow *window, const Player &player) noexcept;
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
const bool outOfBulletRange(const Player &player,
                            const Bullet &bullet) noexcept;
void onPlayerTouchGoodSphere(std::vector<glm::vec2> &goodSpherePositions,
                             Player &player) noexcept;
void onPlayerTouchBadSphere(const std::vector<glm::vec2> &badSpherePositions,
                            Player &player) noexcept;

struct WindowUserData {
  bool mouseButtonPressed{false};
};

int main() {
  const RaiiGlfw raiiGlfw{};

  GLFWwindow *window = glfwCreateWindow(800, 600, "Lab 3", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  WindowUserData userData{};
  glfwSetWindowUserPointer(window, &userData);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  const Scene scene{};

  SceneData data{
      .goodSpherePositions{
          {-.5f, -.5f}, {.2f, -.3f}, {-.6f, .4f}, {.7f, .9f}, {.5f, .0f}},
      .badSpherePositions{
          {-.7f, -.3f}, {.3f, .6f}, {-.5f, .7f}, {.8f, -.1f}, {.2f, -.7f}}};

  while (!glfwWindowShouldClose(window)) {
    std::cout << data.player.speed << std::endl;
    data.player = updatePlayer(window, data.player);

    if (userData.mouseButtonPressed) {
      userData.mouseButtonPressed = false;

      if (!data.bullet.visible) {
        data.bullet = Bullet{.visible{true},
                             .position{data.player.position},
                             .angleDeg{data.player.angleDeg}};
      }
    }

    if (data.bullet.visible) data.bullet = moveBullet(data.bullet);

    if (outOfBulletRange(data.player, data.bullet)) data.bullet.visible = false;

    onPlayerTouchGoodSphere(data.goodSpherePositions, data.player);

    onPlayerTouchBadSphere(data.badSpherePositions, data.player);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.render(viewAspectRatio(), data);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width,
                             int height) noexcept {
  glViewport(0, 0, width, height);
}

const Player updatePlayer(GLFWwindow *window, const Player &player) noexcept {
  Player newPlayer{player};
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    newPlayer = moveForward(newPlayer);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    newPlayer = moveBackward(newPlayer);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    newPlayer = turnLeft(newPlayer);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    newPlayer = turnRight(newPlayer);
  return newPlayer;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  WindowUserData *userData =
      static_cast<WindowUserData *>(glfwGetWindowUserPointer(window));

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    userData->mouseButtonPressed = true;
}

const bool outOfBulletRange(const Player &player,
                            const Bullet &bullet) noexcept {
  return glm::distance(player.position, bullet.position) > 2.f;
}

void onPlayerTouchGoodSphere(std::vector<glm::vec2> &goodSpherePositions,
                             Player &player) noexcept {
  std::vector<glm::vec2> spherePositions{};

  for (const auto &position : goodSpherePositions) {
    if (glm::distance(player.position, position) <= 0.15f)
      player.speed += 0.00015f;
    else spherePositions.push_back(position);
  }

  goodSpherePositions = spherePositions;
}

void onPlayerTouchBadSphere(const std::vector<glm::vec2> &badSpherePositions,
                            Player &player) noexcept {
  for (const auto &position : badSpherePositions) {
    if (glm::distance(player.position, position) <= 0.15f)
      player.speed = std::max(player.speed - 0.00015f, 0.00010f);
  }
}