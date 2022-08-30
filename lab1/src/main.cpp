// Based on http://www.glfw.org and on https://github.com/VictorGordan/opengl-tutorials 
/*
(C) Bedrich Benes 2022
Purdue University
bbenes@purdue.edu
*/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"


GLuint compileShaders()
{
	// Vertex Shader
	const char* vsSrc = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform float scale;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(scale* aPos.x, scale* aPos.y, scale* aPos.z, 1.0);\n"
		"}\0";

	// Fragment Shader
	const char* fsSrc = "#version 330 core\n"
		"out vec4 col;\n"
		"uniform vec4 color;\n"
		"void main()\n"
		"{\n"
		"   col = color;\n"
		"}\n\0";

	// Create VS object
	GLuint vs{ glCreateShader(GL_VERTEX_SHADER) };
	// Attach VS src to the Vertex Shader Object
	glShaderSource(vs, 1, &vsSrc, NULL);
	// Compile the vs
	glCompileShader(vs);

	// The same for FS
	GLuint fs{ glCreateShader(GL_FRAGMENT_SHADER) };
	glShaderSource(fs, 1, &fsSrc, NULL);
	glCompileShader(fs);

	// Get shader program object
	GLuint shaderProg{ glCreateProgram() };
	// Attach both vs and fs
	glAttachShader(shaderProg, vs);
	glAttachShader(shaderProg, fs);
	// Link all
	glLinkProgram(shaderProg);

	// Clear the VS and FS objects
	glDeleteShader(vs);
	glDeleteShader(fs);
	return shaderProg;
}

void buildScene(GLuint& VBO, GLuint& VAO) // return VBO and VAO values
{
	// Vertices coordinates
	const glm::mediump_float K{ 0.5f };
	glm::vec3 vertices[]
	{
		glm::vec3{ -K, -K, 0.0f },
		glm::vec3{ K, -K, 0.0f },
		glm::vec3{ 0,  K, 0.0f }
	};

	// make VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// bind it
	glBindVertexArray(VAO);

	// bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// send the data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure the attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::mediump_float), (void*)0);
	// Make it valid
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Quit when ESC is released
static void KbdCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main()
{
	if (!glfwInit()) return -1;

	// negotiate with OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// make OpenGL windo
	GLFWwindow* window{ glfwCreateWindow(800, 800, "Simple", NULL, NULL) };
	// is all OK?
	if (window == NULL)
	{
		std::cout << "Cannot open GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Paste the window to the current context
	glfwMakeContextCurrent(window);

	// Load GLAD to configure OpenGL
	gladLoadGL();
	// Set the viewport
	glViewport(0, 0, 800, 800);

	// Vertex array object and vertex buffer object indices 
	GLuint VAO0, VBO0;

	// once the OpenGL context is done, build the scene and compile shaders
	buildScene(VBO0, VAO0);
	GLuint shaderProg0{ compileShaders() };

	// Backg color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Use shader
	glUseProgram(shaderProg0);
	// Bind the VAO
	glBindVertexArray(VAO0);
	glPointSize(5);

	// Vertex array object and vertex buffer object indices 
	GLuint VAO1, VBO1;

	// once the OpenGL context is done, build the scene and compile shaders
	buildScene(VBO1, VAO1);
	GLuint shaderProg1{ compileShaders() };

	// Backg color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Use shader
	glUseProgram(shaderProg1);
	// Bind the VAO
	glBindVertexArray(VAO1);
	glPointSize(5);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

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
	while (!glfwWindowShouldClose(window))
	{
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
		glUniform4f(glGetUniformLocation(shaderProg0, "color"), color0[0], color0[1], color0[2], color0[3]);

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
		glUniform4f(glGetUniformLocation(shaderProg1, "color"), color1[0], color1[1], color1[2], color1[3]);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGUI window creation
		ImGui::Begin("CS 535");
		// Text that appears in the window
		ImGui::Text("Let there be OpenGL!");
		// Checkbox that appears in the window
		ImGui::Checkbox("Draw Triangle0", &drawTriangle0);
		ImGui::Checkbox("Fill Triangle0", &fillTriangle0);
		// Slider that appears in the window
		ImGui::SliderFloat("Scale0", &scale0, 0.5f, 2.0f);
		// Fancy color editor that appears in the window
		ImGui::ColorEdit4("Color0", color0);
		// Checkbox that appears in the window
		ImGui::Checkbox("Draw Triangle1", &drawTriangle1);
		ImGui::Checkbox("Fill Triangle1", &fillTriangle1);
		// Slider that appears in the window
		ImGui::SliderFloat("Scale1", &scale1, 0.5f, 2.0f);
		// Fancy color editor that appears in the window
		ImGui::ColorEdit4("Color1", color1);
		// Ends the window
		ImGui::End();

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// make sure events are served
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &VAO0);
	glDeleteBuffers(1, &VBO0);
	glDeleteProgram(shaderProg0);
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProg1);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
