#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "player.h"
#include "raii-glfw.h"
#include "scene.h"
#include "shader.h"
#include "utils.h"

void framebufferSizeCallback(GLFWwindow *window, int width,
                             int height) noexcept;
Player updatePlayer(GLFWwindow *window, const Player &player);

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

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  const Scene scene{};

  SceneData data{
      .player{},
      .goodSphereLocations{
          {-5.f, -5.f}, {2.f, -3.f}, {-6.f, 4.f}, {7.f, 9.f}, {5.f, 0.f}},
      .badSphereLocations{
          {-7.f, -3.f}, {3.f, 6.f}, {-5.f, 7.f}, {8.f, -1.f}, {2.f, -7.f}}};

  while (!glfwWindowShouldClose(window)) {
    data.player = updatePlayer(window, data.player);

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

Player updatePlayer(GLFWwindow *window, const Player &player) {
  Player newPlayer{player};
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    newPlayer = moveForward(player);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    newPlayer = moveBackward(player);
  return newPlayer;
}