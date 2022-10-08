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
void onPlayerTouchGoodSphere(std::vector<SphereData> &spheres,
                             Player &player) noexcept;
void onPlayerTouchBadSphere(const std::vector<SphereData> &spheres,
                            Player &player) noexcept;
void onBulletTouchSphere(std::vector<SphereData> &spheres,
                         Bullet &bullet) noexcept;
void updateHitSpheres(std::vector<SphereData> &spheres) noexcept;

struct WindowUserData {
  bool mouseButtonPressed{false};
};

int main() {
  const RaiiGlfw raiiGlfw{};

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
  glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
  GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Lab 3",
                                        glfwGetPrimaryMonitor(), nullptr);
  if (window == nullptr) {
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

  const glm::vec4 goodColor{0.f, 0.45f, 0.2f, 1.f};
  const glm::vec4 badColor{045.f, 0.f, 0.2f, 1.f};
  SceneData data{.goodSpheres{{
                     {.position{-.5f, -.5f}, .color{goodColor}},
                     {.position{.2f, -.3f}, .color{goodColor}},
                     {.position{-.6f, .4f}, .color{goodColor}},
                     {.position{.7f, .9f}, .color{goodColor}},
                     {.position{.5f, .0f}, .color{goodColor}},
                 }},
                 .badSpheres{{
                     {.position{-.7f, -.3f}, .color{badColor}},
                     {.position{.3f, .6f}, .color{badColor}},
                     {.position{-.5f, .7f}, .color{badColor}},
                     {.position{.8f, -.1f}, .color{badColor}},
                     {.position{.2f, -.7f}, .color{badColor}},
                 }}};

  while (!glfwWindowShouldClose(window)) {
    data.player = updatePlayer(window, data.player);

    if (userData.mouseButtonPressed) {
      userData.mouseButtonPressed = false;

      if (data.player.remainingBulletCount > 0 && !data.bullet.visible) {
        data.player.remainingBulletCount--;
        data.bullet = Bullet{.visible{true},
                             .position{data.player.position},
                             .angleDeg{data.player.angleDeg}};
      }
    }

    if (data.bullet.visible) data.bullet = moveBullet(data.bullet);

    if (outOfBulletRange(data.player, data.bullet)) data.bullet.visible = false;

    onPlayerTouchGoodSphere(data.goodSpheres, data.player);

    onPlayerTouchBadSphere(data.badSpheres, data.player);

    onBulletTouchSphere(data.goodSpheres, data.bullet);
    onBulletTouchSphere(data.badSpheres, data.bullet);

    updateHitSpheres(data.goodSpheres);
    updateHitSpheres(data.badSpheres);

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

void onPlayerTouchGoodSphere(std::vector<SphereData> &spheres,
                             Player &player) noexcept {
  std::vector<SphereData> newSpheres{};

  for (const auto &sphere : spheres) {
    if (!sphere.hit && glm::distance(player.position, sphere.position) <= 0.15f)
      player.speed += 0.00015f;
    else newSpheres.push_back(sphere);
  }

  spheres = newSpheres;
}

void onPlayerTouchBadSphere(const std::vector<SphereData> &spheres,
                            Player &player) noexcept {
  for (const auto &sphere : spheres) {
    if (!sphere.hit && glm::distance(player.position, sphere.position) <= 0.15f)
      player.speed = std::max(player.speed - 0.00015f, 0.00010f);
  }
}

void onBulletTouchSphere(std::vector<SphereData> &spheres,
                         Bullet &bullet) noexcept {
  if (!bullet.visible) return;

  for (auto &sphere : spheres) {
    if (!sphere.hit &&
        glm::distance(bullet.position, sphere.position) <= 0.1f) {
      bullet.visible = false;
      sphere.hit = true;
    }
  }
}

void updateHitSpheres(std::vector<SphereData> &spheres) noexcept {
  std::vector<SphereData> newSpheres{};
  for (const auto &sphere : spheres) {
    if (sphere.hit) newSpheres.push_back(shrink(sphere));
    else newSpheres.push_back(sphere);
  }

  spheres = newSpheres;
}