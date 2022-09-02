#include <array>
#include <algorithm>
#include <iostream>
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
    float scale;
    glm::mediump_float color[4]; // TODO: Use std::array
    // TODO: RenderObject renderObject;
};

void initializeImGui(GLFWwindow* window);
void renderGl(const std::span<const Triangle>& triangles,
              const std::array<GLuint, 2>& shaderPrograms,
              const std::array<GLuint, 2>& VAOs);
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

    const GLsizei size = 2;

    std::array<GLuint, size> shaderPrograms{};
    std::generate(shaderPrograms.begin(), shaderPrograms.end(),
                  [] { return glCreateProgram(); });
    for (const auto& program : shaderPrograms) {
        attachShader(program, "triangle.vert", GL_VERTEX_SHADER);
        attachShader(program, "triangle.frag", GL_FRAGMENT_SHADER);
    }

    std::array<GLuint, size> VAOs{};
    std::array<GLuint, size> VBOs{};

    glGenVertexArrays(size, VAOs.data());
    glGenBuffers(size, VBOs.data());

    buildTriangleVertices(VAOs.at(0), VBOs.at(0));

    // Backg color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // Use shader
    glUseProgram(shaderPrograms.at(0));
    // Bind the VAO
    glBindVertexArray(VAOs.at(0));
    glPointSize(5);

    buildTriangleVertices(VAOs.at(1), VBOs.at(1));

    // Backg color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // Use shader
    glUseProgram(shaderPrograms.at(1));
    // Bind the VAO
    glBindVertexArray(VAOs.at(1));
    glPointSize(5);

    initializeImGui(window);

    std::array<Triangle, size> triangles{
        Triangle{
            .border{ true },
            .fill{ false },
            .scale{ 1.0f },
            .color{ 0.2f, 0.2f, 0.8f, 1.0f }
        },
        Triangle{
            .border{ true },
            .fill{ true },
            .scale{ 0.5f },
            .color{ 0.2f, 0.2f, 0.8f, 1.0f }
        },
    };

    glfwSetKeyCallback(window, KbdCallback); // set keyboard callback to quit

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        renderGl(triangles, shaderPrograms, VAOs);

        renderGui(triangles);
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // make sure events are served
        glfwPollEvents();
    }

    // Cleanup
    cleanupGl(window, size, shaderPrograms, VAOs, VBOs);
    return 0;
}


// Quit when ESC is released
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

void renderGl(const std::span<const Triangle>& triangles,
              const std::array<GLuint, 2>& shaderPrograms,
              const std::array<GLuint, 2>& VAOs) {
    // Clean the window
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t idx = 0; const auto & triangle : triangles) {
        glUseProgram(shaderPrograms.at(idx));
        glBindVertexArray(VAOs.at(idx));

        if (triangle.border) {
            glDrawArrays(GL_POINTS, 0, 3);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }

        if (triangle.fill) glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniform1f(glGetUniformLocation(shaderPrograms.at(idx), "scale"),
                    triangle.scale);
        glUniform4f(glGetUniformLocation(shaderPrograms.at(idx), "color"),
                    triangle.color[0], triangle.color[1], triangle.color[2],
                    triangle.color[3]);

        idx++;
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
        ImGui::SliderFloat(("Scale " + std::to_string(idx)).c_str(),
                           &triangle.scale, 0.5f, 2.0f);
        ImGui::ColorEdit4(("Color " + std::to_string(idx)).c_str(),
                          triangle.color);

        ++idx;
    }
    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}