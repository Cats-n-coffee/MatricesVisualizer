#include <iostream>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw_gl3.h"
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helpers/ShaderProgram.h"

static void logString(const char* message)
{
	std::cout << message << std::endl;
}

int width = 1024;
int height = 768;

float vertices[12] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5, 0.5f, 0.0f,
};

unsigned int indices[6] = {
	0, 1, 2, 0, 2, 3
};

int main()
{
	// Create window
	GLFWwindow* window;

	if (!glfwInit()) {
		logString("GLFW could not initialize");
		return -1;
	}
	
	// Set OpenGL minumum version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Matrices Visualizer", NULL, NULL);

	if (!window) {
		glfwTerminate();
		logString("Window could not initialize");
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// ImGui
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);

	ImGui::StyleColorsDark();

	// Add GLEW for OpenGL access
	glewExperimental = GL_TRUE;

	GLenum glewInitialize = glewInit();
	if (glewInitialize != GLEW_OK) {
		std::cout << "Error glewInit: " << glewGetErrorString(glewInitialize);
		glfwTerminate();
		return -1;
	}

	// VAO, VBO
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ShaderProgram shaderProgram = ShaderProgram(
		"resources/vertex.shader",
		"resources/fragment.shader"
	);

	// MVP
	glm::vec3 modelVector = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), modelVector);

	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(1.0f, 1.0f, 0.5f), // eye
		glm::vec3(0.0f, 0.0f, 0.0f), // center
		glm::vec3(0.0f, 1.0f, 0.0f) // up/down - use y only?
	);

	glm::mat4 orthoProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
	// TODO: Add perspective projection
	// TODO: Add checkbox for choosing projection

	// TODO: add color to vertices
	// TODO: add texture

	glm::mat4 modelViewProjection = orthoProjection * viewMatrix * modelMatrix;
	unsigned int mvpLocation = glGetUniformLocation(shaderProgram.getProgramId(), "modelViewProjection");

	float slider = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glBindVertexArray(VAO);
		glClearColor(0.1f, 0.1f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();

		// TODO: send MVP to shader with uniform
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &modelViewProjection[0][0]);

		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, (void*)0);

		// Dear Im GUI
		ImVec2 windowSize = { 500, 400 };
		ImGui::SetNextWindowSize(windowSize);
		bool imGuiwindow = ImGui::Begin("Im GUI Hello");
		if (imGuiwindow)
		{
			// Model Matrix
			ImGui::SliderFloat("Model Transform X", &modelVector.x, 0.0f, 10.0f, "%.1f", 0.1f);
			ImGui::Text("Model Transform X: %.1f", modelVector.x);

			ImGui::SliderFloat("Model Transform Y", &modelVector.y, 0.0f, 10.0f, "%.1f", 0.1f);
			ImGui::Text("Model Transform Y: %.1f", modelVector.y);

			ImGui::SliderFloat("Model Transform Z", &modelVector.z, 0.0f, 10.0f, "%.1f", 0.1f);
			ImGui::Text("Model Transform Z: %.1f", modelVector.z);

			// TODO: add sliders for view matrix
			
			// TODO: add sliders for ortho projection

			// TODO: add checkbox for projection ortho/perspective

			// TODO: add sliders for perspective projection

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glfwSwapBuffers(window);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}