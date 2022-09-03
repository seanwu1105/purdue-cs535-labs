#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <variant>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "gl.h"

struct Triangle {
    bool border;
    bool fill;
    float pointSize;
    float scale;
    std::array<glm::mediump_float, 4> color;
    RenderObject renderObject;
};

template<size_t N>
std::array<Triangle, N> initializeTriangles();
void initializeImGui(GLFWwindow* window);
void renderGl(const std::span<const Triangle>& triangles);
void renderGui(const std::span<Triangle>& triangles);
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods);

int main() {
    const auto initGlResult{ initGl() };
    if (std::holds_alternative<ErrorResult>(initGlResult)) {
        std::cout << std::get<ErrorResult>(initGlResult).message << std::endl;
        return -1;
    }

    const auto window{
        std::get<SuccessResult<GLFWwindow* const>>(initGlResult).value
    };

    glViewport(0, 0, 800, 800);

    const GLsizei size{ 50 };
    auto triangles{ initializeTriangles<size>() };

    // Set background color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    initializeImGui(window);

    glfwSetKeyCallback(window, KbdCallback);

    while (!glfwWindowShouldClose(window)) {
        renderGl(triangles);
        renderGui(triangles);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::array<RenderObject, triangles.size()> renderObjects{};
    std::transform(triangles.begin(), triangles.end(), renderObjects.begin(),
                   [](const auto& triangle) { return triangle.renderObject; });
    cleanupGl(window, renderObjects);
    return 0;
}

template<size_t N>
std::array<Triangle, N> initializeTriangles() {
    std::array<Triangle, N> triangles{};
    std::array<int, N> indices{};
    std::iota(indices.begin(), indices.end(), 0);
    std::transform(
        indices.begin(), indices.end(), triangles.begin(),
        [](const auto& idx) { return Triangle{
            .border{ true },
            .fill{ false },
            .pointSize { 20.0f / (idx + 1)},
            .scale{ 2.0f / (idx + 1) },
            .color{
                static_cast<glm::mediump_float>(std::abs(std::sin(idx))),
                static_cast<glm::mediump_float>(std::abs(std::cos(idx + 0.5))),
                static_cast<glm::mediump_float>(std::abs(std::cos(idx))),
                1.0f},
            .renderObject {.shaderProgram{ glCreateProgram() }}
        }; });

    for (auto& triangle : triangles) {
        attachShader(triangle.renderObject.shaderProgram,
                     "triangle.vert", GL_VERTEX_SHADER);
        attachShader(triangle.renderObject.shaderProgram,
                     "triangle.frag", GL_FRAGMENT_SHADER);
        glGenVertexArrays(1, &triangle.renderObject.VAO);
        glGenBuffers(1, &triangle.renderObject.VBO);

        const glm::mediump_float k{ 0.5f };

        const std::array<const glm::vec3, 3> vertices{
            glm::vec3{ -k, -k, 0.0f },
            glm::vec3{ k, -k, 0.0f },
            glm::vec3{ 0,  k, 0.0f },
        };
        buildTriangleVertices(triangle.renderObject, vertices);
    }

    return triangles;
}

void initializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderGl(const std::span<const Triangle>& triangles) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& triangle : triangles) {
        glUseProgram(triangle.renderObject.shaderProgram);
        glBindVertexArray(triangle.renderObject.VAO);

        glPointSize(triangle.pointSize);
        if (triangle.border) {
            glDrawArrays(GL_POINTS, 0, 3);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }

        if (triangle.fill) glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniform1f(glGetUniformLocation(triangle.renderObject.shaderProgram,
                                         "scale"),
                    triangle.scale);
        glUniform4f(glGetUniformLocation(triangle.renderObject.shaderProgram,
                                         "color"),
                    triangle.color.at(0), triangle.color.at(1),
                    triangle.color.at(2), triangle.color.at(3));
    }
}

void renderGui(const std::span<Triangle>& triangles) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("CS 535");
    ImGui::Text("Let there be OpenGL!");
    for (size_t idx = 0; auto & triangle : triangles) {
        ImGui::Text(("Triangle " + std::to_string(idx)).c_str());
        ImGui::Checkbox(("Border " + std::to_string(idx)).c_str(),
                        &triangle.border);
        ImGui::Checkbox(("Fill " + std::to_string(idx)).c_str(),
                        &triangle.fill);
        ImGui::SliderFloat(("Point Size " + std::to_string(idx)).c_str(),
                           &triangle.pointSize, 5.0f, 20.0f);
        ImGui::SliderFloat(("Scale " + std::to_string(idx)).c_str(),
                           &triangle.scale, 0.5f, 2.0f);
        ImGui::ColorEdit4(("Color " + std::to_string(idx)).c_str(),
                          triangle.color.data());

        ++idx;
    }
    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Quit when ESC is released
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}