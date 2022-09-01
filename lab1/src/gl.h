#pragma once

#include <variant>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "result.h"

std::variant<SuccessResult<GLFWwindow*>, FailureResult> initializeGl();

//void cleanupGl(GLFWwindow* window,
//			   std::iterator<GLuint> shaderPrograms,
//			   std::iterator<GLuint> VAOs,
//			   std::vector<GLuint> VBOs);