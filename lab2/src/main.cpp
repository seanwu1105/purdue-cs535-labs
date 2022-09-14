/*
 Ruled surface
(C) Bedrich Benes 2022
Purdue University
bbenes@purdue.edu
*/

#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "triangle.h" //triangles
#include "helper.h"         
#include "objGen.h" //to save OBJ file format for 3D printing
#include "trackball.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "glfw3.lib")


TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;

std::vector <TriangleC> tri;   //all the triangles will be stored here
std::string filename = "geometry.obj";

GLuint points = 0; //number of points to display the object
int steps = 12;//# of subdivisions
int pointSize = 5;
int lineWidth = 1;
GLdouble mouseX, mouseY;

//Vertex array object and vertex buffer object indices 
GLuint visualizationVAO, visualizationVBO;


inline void AddVertex(std::vector <GLfloat>* a, glm::vec3 A) {
    a->push_back(A[0]); a->push_back(A[1]); a->push_back(A[2]);
}


auto P(GLfloat u, std::map<GLfloat, GLfloat>& transformedEditorVertices) {
    if (transformedEditorVertices.contains(u)) return transformedEditorVertices.at(u);
    if (transformedEditorVertices.empty()) return 0.0f;
    if (u < transformedEditorVertices.cbegin()->first) return 0.0f;
    if (u > std::prev(transformedEditorVertices.end())->first) return 0.0f;

    const auto end_point{ transformedEditorVertices.upper_bound(u) };
    const auto start_point{ std::prev(end_point) };

    return ((end_point->second - start_point->second) / (end_point->first - start_point->first)) * (u - start_point->first) + start_point->second;
}

inline glm::vec3 S(GLfloat u, GLfloat v, std::map<GLfloat, GLfloat>& transformedEditorVertices) {
    return glm::vec3{ P(u, transformedEditorVertices) * sin(2 * M_PI * v), u, P(u, transformedEditorVertices) * cos(2 * M_PI * v) };
}

void createRuled(std::vector <GLfloat>* vv, int step_count, std::map<GLfloat, GLfloat>& editorVertices) {
    std::map<GLfloat, GLfloat> transformed{};

    for (const auto& [key, value] : editorVertices)
        transformed[(key + 1) / 2] = (value + 1) / 2;

    GLfloat step = 1.f / step_count;
    for (int i = 0; i < step_count; i++) {
        for (int j = 0; j < step_count; j++) {
            //lower triangle
            AddVertex(vv, S(i * step, j * step, transformed));
            AddVertex(vv, S((i + 1) * step, j * step, transformed));
            AddVertex(vv, S((i + 1) * step, (j + 1) * step, transformed));
            //upper triangle
            AddVertex(vv, S(i * step, j * step, transformed));
            AddVertex(vv, S((i + 1) * step, (j + 1) * step, transformed));
            AddVertex(vv, S(i * step, (j + 1) * step, transformed));
        }
    }
}


int CompileShaders() {
    //Vertex Shader
    const char* vsSrc = "#version 330 core\n"
        "layout (location = 0) in vec4 iPos;\n"
        "uniform mat4 modelview;\n"
        "void main()\n"
        "{\n"
        "   vec4 oPos=modelview*iPos;\n"
        "   gl_Position = vec4(oPos.x, oPos.y, oPos.z, oPos.w);\n"
        "}\0";

    //Fragment Shader
    const char* fsSrc = "#version 330 core\n"
        "out vec4 col;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   col = color;\n"
        "}\n\0";

    //Create VS object
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    //Attach VS src to the Vertex Shader Object
    glShaderSource(vs, 1, &vsSrc, NULL);
    //Compile the vs
    glCompileShader(vs);

    //The same for FS
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(fs);

    //Get shader program object
    GLuint shaderProg = glCreateProgram();
    //Attach both vs and fs
    glAttachShader(shaderProg, vs);
    glAttachShader(shaderProg, fs);
    //Link all
    glLinkProgram(shaderProg);

    //Clear the VS and FS objects
    glDeleteShader(vs);
    glDeleteShader(fs);
    return shaderProg;
}

void buildScene(GLuint& VBO, GLuint& VAO, int step_count, std::map<GLfloat, GLfloat>& editorVertices) {
    std::vector<GLfloat> v;
    createRuled(&v, step_count, editorVertices);
    //now get it ready for saving as OBJ
    tri.clear();
    for (unsigned int i = 0; i < v.size(); i += 9) { //stride 3 - 3 vertices per triangle
        TriangleC tmp;
        glm::vec3 a, b, c;
        a = glm::vec3(v[i], v[i + 1], v[i + 2]);
        b = glm::vec3(v[i + 3], v[i + 4], v[i + 5]);
        c = glm::vec3(v[i + 6], v[i + 7], v[i + 8]);
        tmp.Set(a, b, c); //store them for 3D export
        tri.push_back(tmp);
    }

    //make VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //bind it
    glBindVertexArray(VAO);

    //bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //send the data to the GPU
    points = v.size();
    glBufferData(GL_ARRAY_BUFFER, points * sizeof(GLfloat), &v[0], GL_STATIC_DRAW);

    //Configure the attributes
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //Make it valid
    glEnableVertexAttribArray(0);

    v.clear(); //no need for the data, it is on the GPU now

}

//Quit when ESC is released
static void windowKbdCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

struct WindowUserData {
    ImGuiContext* guiContext;
};

void windowCursorPosCallback(GLFWwindow* window, double x, double y) {
    const auto windowData{ (WindowUserData*)glfwGetWindowUserPointer(window) };
    if (windowData == nullptr) return;
    ImGui::SetCurrentContext(windowData->guiContext);

    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu
    //now process them
    mouseX = x;
    mouseY = y;
    //we need to perform an action only if a button is pressed
    if (mouseLeft)  trackball.Rotate(mouseX, mouseY);
    if (mouseMid)   trackball.Translate(mouseX, mouseY);
    if (mouseRight) trackball.Zoom(mouseX, mouseY);
}

//set the variables when the button is pressed or released
void windowMouseButtonCallback(GLFWwindow* window, int button, int state, int mods) {
    const auto windowData{ (WindowUserData*)glfwGetWindowUserPointer(window) };
    if (windowData == nullptr) return;
    ImGui::SetCurrentContext(windowData->guiContext);

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, state);
    if (io.WantCaptureMouse) return; //make sure you do not call this callback when over a menu

    //process them
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS) {
        trackball.Set(window, true, mouseX, mouseY);
        mouseLeft = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE) {
        trackball.Set(window, false, mouseX, mouseY);
        mouseLeft = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_PRESS) {
        trackball.Set(window, true, mouseX, mouseY);
        mouseMid = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_RELEASE) {
        trackball.Set(window, true, mouseX, mouseY);
        mouseMid = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS) {
        trackball.Set(window, true, mouseX, mouseY);
        mouseRight = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_RELEASE) {
        trackball.Set(window, true, mouseX, mouseY);
        mouseRight = false;
    }
}


GLFWwindow* createEditorWindow(int size) {
    if (glfwInit() != GLFW_TRUE) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(size, size, "XY Plane", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) return nullptr;
    glViewport(0, 0, 800, 800);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    return window;
}

void buildEditorAxisVertices(const GLuint VAO, const GLuint VBO) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const std::array<const glm::vec3, 4> vertices{
        glm::vec3{-1.0f,0.0f,0.0f},
        glm::vec3{1.0f,0.0f,0.0f},
        glm::vec3{0.0f,-1.0f,0.0f},
        glm::vec3{0.0f,1.0f,0.0f},
    };

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

auto createShaderProgram(const GLchar* const* vertexShaderSrc, const GLchar* const* fragmentShaderSrc) {
    const auto shaderProgram{ glCreateProgram() };

    const std::array<GLuint, 2> shaders{
        glCreateShader(GL_VERTEX_SHADER),
        glCreateShader(GL_FRAGMENT_SHADER)
    };
    glShaderSource(shaders.at(0), 1, vertexShaderSrc, nullptr);
    glShaderSource(shaders.at(1), 1, fragmentShaderSrc, nullptr);
    for (const auto& shader : shaders) {
        glCompileShader(shader);
        glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }
    glLinkProgram(shaderProgram);

    return shaderProgram;
}

void renderEditorAxes(GLFWwindow* window, const GLuint shaderProgram, const GLuint VAO, const GLuint VBO) {
    glfwMakeContextCurrent(window);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glDrawArrays(GL_LINES, 0, 4);
}

void updateVertexBufferData(std::map<GLfloat, GLfloat>& yxPairs) {
    std::vector<glm::vec2> vertices{};
    for (const auto& [y, x] : yxPairs) vertices.push_back(glm::vec2{ x, y });
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

struct EditorWindowUserData {
    ImGuiContext* guiContext;
    GLfloat lastClickedXPos;
    GLfloat lastClickedYPos;
};

void renderEditorVertices(GLFWwindow* window, const GLuint shaderProgram,
                          const GLuint VAO, const GLuint VBO,
                          std::map<GLfloat, GLfloat>& yxPairs,
                          std::vector<GLfloat>& yxPairInsertionOrder,
                          bool& shouldRemoveLastVertex,
                          bool& shouldClearAllVertices,
                          const int windowSize) {
    glfwMakeContextCurrent(window);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const auto data{ (EditorWindowUserData*)glfwGetWindowUserPointer(window) };
    if (data && data->lastClickedXPos >= 0 && data->lastClickedYPos >= 0) {
        const auto normalizedX = (GLfloat)(data->lastClickedXPos / ((GLfloat)windowSize / 2)) - 1;
        const auto normalizedY = -(data->lastClickedYPos / ((GLfloat)windowSize / 2) - 1);
        yxPairs[normalizedY] = normalizedX;
        yxPairInsertionOrder.push_back(normalizedY);
        data->lastClickedXPos = -1;
        data->lastClickedYPos = -1;

        updateVertexBufferData(yxPairs);
    }

    if (shouldRemoveLastVertex) {
        shouldRemoveLastVertex = false;
        if (!yxPairInsertionOrder.empty()) {
            const auto lastY = yxPairInsertionOrder.back();
            yxPairInsertionOrder.pop_back();
            yxPairs.erase(lastY);

            updateVertexBufferData(yxPairs);
        }
    }

    if (shouldClearAllVertices) {
        shouldClearAllVertices = false;
        yxPairs.clear();
        yxPairInsertionOrder.clear();

        updateVertexBufferData(yxPairs);
    }

    glDrawArrays(GL_LINE_STRIP, 0, yxPairs.size());
}

void renderEditorGui(ImGuiContext* guiContext, bool& shouldRemoveLastVertex, bool& shouldClearAllVertices) {
    ImGui::SetCurrentContext(guiContext);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Controllers");
    if (ImGui::Button("Clear All Points")) shouldClearAllVertices = true;
    if (ImGui::Button("Delete Last Point")) shouldRemoveLastVertex = true;
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void editorMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto data = (EditorWindowUserData*)glfwGetWindowUserPointer(window);
    if (data == nullptr) return;
    ImGui::SetCurrentContext(data->guiContext);

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action);
    if (io.WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        double x{}, y{};
        glfwGetCursorPos(window, &x, &y);
        data->lastClickedXPos = (GLfloat)x;
        data->lastClickedYPos = (GLfloat)y;
    }
}

int main() {
    glfwInit();

    //negotiate with the OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make OpenGL window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Visualization", NULL, NULL);
    //is all OK?
    if (window == NULL) {
        std::cout << "Cannot open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //Paste the window to the current context
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    //Set the viewport
    glViewport(0, 0, 800, 800);

    std::map<GLfloat, GLfloat> editorVertices{};
    auto prevEditorVertices{ editorVertices };
    std::vector<GLfloat> editorVertexInsertionOrder{};
    buildScene(visualizationVBO, visualizationVAO, steps, editorVertices);
    int shaderProg = CompileShaders();
    GLint modelviewParameter = glGetUniformLocation(shaderProg, "modelview");

    //Background color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //Use shader
    glUseProgram(shaderProg);
    glPointSize(pointSize);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    const auto windowGuiContext{ ImGui::CreateContext() };
    WindowUserData windowData{ windowGuiContext };
    glfwSetWindowUserPointer(window, &windowData);
    ImGui::SetCurrentContext(windowGuiContext);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool drawScene = true;
    float color[4] = { 0.8f, 0.8f, 0.2f, 1.0f };
    //send the color to the fragment shader
    glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);


    glfwSetKeyCallback(window, windowKbdCallback); //set keyboard callback to quit
    glfwSetCursorPosCallback(window, windowCursorPosCallback);
    glfwSetMouseButtonCallback(window, windowMouseButtonCallback);

    const int editorWindowSize = 800;
    const auto editorWindow = createEditorWindow(editorWindowSize);
    if (editorWindow == nullptr) {
        std::cout << "Cannot create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    GLuint editorAxisVAO{}, editorAxisVBO{};
    glGenVertexArrays(1, &editorAxisVAO);
    glGenBuffers(1, &editorAxisVBO);
    const auto editorAxisVertexShaderSrc{
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos, 1.0f);\n"
        "}\n"
    };
    const auto editorAxisFragmentShaderSrc{
        "#version 330 core\n"
        "out vec4 col;\n"
        "void main() {\n"
        "  col = vec4(0.5f, 0.5f, 0.5f, 1.0f);\n"
        "}\n"
    };
    GLuint editorAxisShaderProgram{ createShaderProgram(&editorAxisVertexShaderSrc, &editorAxisFragmentShaderSrc) };
    buildEditorAxisVertices(editorAxisVAO, editorAxisVBO);

    GLuint editorVertexVAO{}, editorVertexVBO{};
    glGenVertexArrays(1, &editorVertexVAO);
    glGenBuffers(1, &editorVertexVBO);
    const auto editorVertexVertexShaderSrc{
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPos;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos, 1.0f, 1.0f);\n"
        "}\n"
    };
    const auto editorVertexFragmentShaderSrc{
        "#version 330 core\n"
        "out vec4 col;\n"
        "void main() {\n"
        "  col = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "}\n"
    };
    GLuint editorVertexShaderProgram{ createShaderProgram(&editorVertexVertexShaderSrc, &editorVertexFragmentShaderSrc) };

    const auto editorGuiContext{ ImGui::CreateContext() };
    ImGui::SetCurrentContext(editorGuiContext);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(editorWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    bool shouldRemoveLastVertex{ false };
    bool shouldClearAllVertices{ false };

    EditorWindowUserData editorWindowData{ editorGuiContext, -1, -1 };
    glfwSetWindowUserPointer(editorWindow, &editorWindowData);
    glfwSetMouseButtonCallback(editorWindow, editorMouseButtonCallback);

    // Main while loop
    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(editorWindow)) {
        glfwMakeContextCurrent(window);
        ImGui::SetCurrentContext(windowGuiContext);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(visualizationVAO);
        glBindBuffer(GL_ARRAY_BUFFER, visualizationVBO);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGUI window creation
        ImGui::Begin("Ruled Surface");
        //checkbox to render or not the scene
        ImGui::Checkbox("Draw Scene", &drawScene);
        //checkbox to render or not the scene
        if (ImGui::Button("Save OBJ")) SaveOBJ(&tri, filename);

        bool needRebuildScene{ false };
        if (prevEditorVertices != editorVertices) {
            needRebuildScene = true;
            prevEditorVertices = editorVertices;
        }
        if (ImGui::SliderInt("Mesh Subdivision", &steps, 1, 100, "%d", 0) || needRebuildScene) {
            buildScene(visualizationVBO, visualizationVAO, steps, editorVertices);
            needRebuildScene = false;
        }
        if (ImGui::SliderInt("point Size", &pointSize, 1, 10, "%d", 0)) {
            glPointSize(pointSize); //set the new point size if it has been changed			
        }
        if (ImGui::SliderInt("line width", &lineWidth, 1, 10, "%d", 0)) {
            glLineWidth(lineWidth); //set the new point size if it has been changed			
        }
        if (ImGui::ColorEdit4("Color", color)) { //set the new color only if it has changed
            glUniform4f(glGetUniformLocation(shaderProg, "color"), color[0], color[1], color[2], color[3]);
        }

        // Ends the window
        ImGui::End();

        //set the projection matrix
        glm::mat4 proj = glm::perspective(65.f, 1.f, 0.01f, 1000.f);
        //set the viewing matrix (looking from [0,0,5] to [0,0,0])
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        //get the modeling matrix from the trackball
        glm::mat4 model = trackball.Set3DViewCameraMatrix();
        //premultiply the modelViewProjection matrix
        glm::mat4 modelView = proj * view * model;
        //and send it to the vertex shader
        glUniformMatrix4fv(modelviewParameter, 1, GL_FALSE, glm::value_ptr(modelView));

        if (drawScene) {
            glDrawArrays(GL_POINTS, 0, points / 3);
            glDrawArrays(GL_TRIANGLES, 0, points / 3);
        }

        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //Swap the back buffer with the front buffer
        glfwSwapBuffers(window);

        glfwMakeContextCurrent(editorWindow);
        glClear(GL_COLOR_BUFFER_BIT);
        renderEditorAxes(editorWindow, editorAxisShaderProgram, editorAxisVAO, editorAxisVBO);
        renderEditorVertices(editorWindow, editorVertexShaderProgram,
                             editorVertexVAO, editorVertexVBO,
                             editorVertices, editorVertexInsertionOrder,
                             shouldRemoveLastVertex,
                             shouldClearAllVertices,
                             editorWindowSize);
        renderEditorGui(editorGuiContext, shouldRemoveLastVertex, shouldClearAllVertices);
        glfwSwapBuffers(editorWindow);

        //make sure events are served
        glfwPollEvents();
    }
    //Cleanup
    glDeleteVertexArrays(1, &visualizationVAO);
    glDeleteBuffers(1, &visualizationVBO);
    glDeleteProgram(shaderProg);
    glfwDestroyWindow(window);
    glfwDestroyWindow(editorWindow);
    glfwTerminate();
    return 0;
}
