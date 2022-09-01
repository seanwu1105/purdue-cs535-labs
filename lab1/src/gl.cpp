#include "gl.h"

std::variant<SuccessResult<GLFWwindow*>, FailureResult> initializeGl() {
	if (!glfwInit()) return FailureResult{ "Fail to init glfw." };

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const auto window{ glfwCreateWindow(800, 800,
										"Simple", nullptr, nullptr) };
	if (window == nullptr) {
		glfwTerminate();
		return FailureResult{ "Cannot open GLFW window." };
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();
	return SuccessResult{ window };
}