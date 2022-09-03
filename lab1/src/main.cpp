#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <variant>
#include <vector>

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
    float rotateDeg;
    std::array<float, 4> color;
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

    const GLsizei size{ 100 };
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
            .fill{ true },
            .pointSize { 20.0f - (idx + 1)},
            .scale{ 2.0f - (0.02f * idx) },
            .rotateDeg{std::fmod(5.0f * idx, 360.0f)},
            .color{
                static_cast<float>(std::abs(std::sin(idx))),
                static_cast<float>(std::abs(std::cos(idx + 0.5))),
                static_cast<float>(std::abs(std::cos(idx))),
                1.0f
            },
            .renderObject {.shaderProgram{ glCreateProgram() }}
        }; });

    for (auto& triangle : triangles) {
        attachShader(triangle.renderObject.shaderProgram,
                     "triangle.vert", GL_VERTEX_SHADER);
        attachShader(triangle.renderObject.shaderProgram,
                     "triangle.frag", GL_FRAGMENT_SHADER);
        glGenVertexArrays(1, &triangle.renderObject.VAO);
        glGenBuffers(1, &triangle.renderObject.VBO);

        const float k{ 0.5f };

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

        glm::mat4 trans{ glm::mat4(1.0f) };
        trans = glm::rotate(trans, glm::radians(triangle.rotateDeg), glm::vec3(
            0.0, 0.0, 1.0
        ));
        trans = glm::scale(trans, glm::vec3(
            triangle.scale, triangle.scale, triangle.scale
        ));
        glUniformMatrix4fv(
            glGetUniformLocation(triangle.renderObject.shaderProgram, "trans"),
            1, GL_FALSE, glm::value_ptr(trans)
        );

        glUniform4f(
            glGetUniformLocation(triangle.renderObject.shaderProgram, "color"),
            triangle.color.at(0), triangle.color.at(1),
            triangle.color.at(2), triangle.color.at(3)
        );
    }
}

void renderGui(const std::span<Triangle>& triangles) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("CS 535");
    ImGui::Text("Let there be OpenGL!");

    static size_t selectedIdx{ 0 };
    const auto comboPreview{ std::to_string(selectedIdx) };
    if (ImGui::BeginCombo("Triangle Selection", comboPreview.c_str())) {
        for (size_t idx{ 0 }; const auto & triangle : triangles) {
            const bool isSelected{ selectedIdx == idx };
            if (ImGui::Selectable(std::to_string(idx).c_str(), isSelected)) {
                selectedIdx = idx;
            }
            if (isSelected) ImGui::SetItemDefaultFocus();

            ++idx;
        }
        ImGui::EndCombo();
    }

    ImGui::Checkbox("Border", &triangles[selectedIdx].border);
    ImGui::Checkbox("Fill", &triangles[selectedIdx].fill);
    ImGui::SliderFloat("Point Size", &triangles[selectedIdx].pointSize,
                       5.0f, 20.0f);
    ImGui::SliderFloat("Scale", &triangles[selectedIdx].scale, 0.0f, 2.0f);
    ImGui::SliderFloat("Rotate", &triangles[selectedIdx].rotateDeg, 0.0f, 360.0f);
    ImGui::ColorEdit4("Color", triangles[selectedIdx].color.data());
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