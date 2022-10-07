#pragma once

#include <random>

#include <glad/glad.h>

const float getViewAspectRatio() noexcept {
  std::array<GLint, 4> viewport{};
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  return {static_cast<float>(viewport.at(2) - viewport.at(0)) /
          (viewport.at(3) - viewport.at(1))};
}