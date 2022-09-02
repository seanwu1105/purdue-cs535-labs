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

void renderGl(GLuint shaderProg0, GLuint VAO0, bool drawTriangle0,
              bool fillTriangle0, float scale0, glm::mediump_float color0[],
              GLuint shaderProg1, GLuint VAO1, bool drawTriangle1,
              bool fillTriangle1, float scale1, glm::mediump_float color1[]) {
    // Clean the window
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProg0);
    glBindVertexArray(VAO0);

    // Draw the scene
    if (drawTriangle0) {
        glDrawArrays(GL_POINTS, 0, 3);
        glDrawArrays(GL_LINE_LOOP, 0, 3);
    }
    if (fillTriangle0) glDrawArrays(GL_TRIANGLES, 0, 3);

    // Export variables to shader
    glUniform1f(glGetUniformLocation(shaderProg0, "scale"), scale0);
    glUniform4f(glGetUniformLocation(shaderProg0, "color"),
                color0[0], color0[1], color0[2], color0[3]);

    glUseProgram(shaderProg1);
    glBindVertexArray(VAO1);

    // Draw the scene
    if (drawTriangle1) {
        glDrawArrays(GL_POINTS, 0, 3);
        glDrawArrays(GL_LINE_LOOP, 0, 3);
    }
    if (fillTriangle1) glDrawArrays(GL_TRIANGLES, 0, 3);

    // Export variables to shader
    glUniform1f(glGetUniformLocation(shaderProg1, "scale"), scale1);
    glUniform4f(glGetUniformLocation(shaderProg1, "color"),
                color1[0], color1[1], color1[2], color1[3]);
}

void renderGui(bool* drawTriangle0, bool* fillTriangle0, float* scale0,
               glm::mediump_float* color0, bool* drawTriangle1,
               bool* fillTriangle1, float* scale1,
               glm::mediump_float* color1) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGUI window creation
    ImGui::Begin("CS 535");
    // Text that appears in the window
    ImGui::Text("Let there be OpenGL!");
    // Checkbox that appears in the window
    ImGui::Checkbox("Draw Triangle0 Border", drawTriangle0);
    ImGui::Checkbox("Fill Triangle0", fillTriangle0);
    // Slider that appears in the window
    ImGui::SliderFloat("Scale0", scale0, 0.5f, 2.0f);
    // Fancy color editor that appears in the window
    ImGui::ColorEdit4("Color0", color0);
    // Checkbox that appears in the window
    ImGui::Checkbox("Draw Triangle1 Border", drawTriangle1);
    ImGui::Checkbox("Fill Triangle1", fillTriangle1);
    // Slider that appears in the window
    ImGui::SliderFloat("Scale1", scale1, 0.5f, 2.0f);
    // Fancy color editor that appears in the window
    ImGui::ColorEdit4("Color1", color1);
    // Ends the window
    ImGui::End();

    // Renders the ImGUI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

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
    std::transform(shaderPrograms.begin(), shaderPrograms.end(),
                   shaderPrograms.begin(),
                   [](GLuint) {return glCreateProgram(); });
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

    bool drawTriangle0{ true };
    bool fillTriangle0{ false };
    float scale0{ 1.0f };
    glm::mediump_float color0[4]{ 0.2f, 0.2f, 0.8f, 1.0f };

    bool drawTriangle1{ true };
    bool fillTriangle1{ false };
    float scale1{ 0.5f };
    glm::mediump_float color1[4]{ 0.2f, 0.2f, 0.8f, 1.0f };

    glfwSetKeyCallback(window, KbdCallback); // set keyboard callback to quit

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        renderGl(shaderPrograms.at(0), VAOs.at(0), drawTriangle0, fillTriangle0, scale0,
                 color0, shaderPrograms.at(1), VAOs.at(1), drawTriangle1, fillTriangle1,
                 scale1, color1);

        renderGui(&drawTriangle0, &fillTriangle0, &scale0, color0,
                  &drawTriangle1, &fillTriangle1, &scale1, color1);
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // make sure events are served
        glfwPollEvents();
    }

    // Cleanup
    cleanupGl(window, size, shaderPrograms, VAOs, VBOs);
    return 0;
}
