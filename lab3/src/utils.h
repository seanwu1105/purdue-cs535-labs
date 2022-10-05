#pragma once

#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const float getViewAspectRatio() {
    GLint viewport[4]{};
    glGetIntegerv(GL_VIEWPORT, viewport);
    return { (float)(viewport[2] - viewport[0]) / (viewport[3] - viewport[1]) };
}