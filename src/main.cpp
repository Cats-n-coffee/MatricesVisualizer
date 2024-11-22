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
	ImVec2 windowSize = { 500, 600 };

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

	// ---- MVP
	// Model
	glm::vec3 modelVector = glm::vec3(1.0f, 1.0f, 1.0f);
	// View
	glm::vec3 viewEye = glm::vec3(0.0f, 0.0f, 1.1f);
	glm::vec3 viewCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 viewUpDown = glm::vec3(0.0f, 1.0f, 0.0f);
	// Perspective
	// Ortho
	float orthoLeft = -2.0f;
	float orthoRight = 2.0f;
	float orthoBottom = -2.0f;
	float orthoTop = 2.0f;
	// Perspective
	float fov = 45.0f;
	
	// Checkbox for choosing projection
	bool showOrthoProjection = true;

	// TODO: add color to vertices
	// TODO: add texture

	unsigned int mvpLocation = glGetUniformLocation(shaderProgram.getProgramId(), "modelViewProjection");

	float slider = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glBindVertexArray(VAO);
		glClearColor(0.1f, 0.1f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();

		// Send MVP to shader with uniform
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), modelVector);
		glm::mat4 viewMatrix = glm::lookAt(
			viewEye, // eye
			viewCenter, // center
			viewUpDown // up/down - use y only?
		);
		glm::mat4 modelViewProjection;

		if (showOrthoProjection)
		{
			glm::mat4 orthoProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, 0.1f, 100.0f);
			modelViewProjection = orthoProjection * viewMatrix * modelMatrix;
		}
		else
		{
			glm::mat4 perspectiveProjection = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);
			modelViewProjection = perspectiveProjection * viewMatrix * modelMatrix;
		}
		
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &modelViewProjection[0][0]);

		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, (void*)0);

		// Dear Im GUI
		ImGui::SetNextWindowSize(windowSize);
		bool imGuiwindow = ImGui::Begin("Im GUI Hello");
		if (imGuiwindow)
		{
			// Model Matrix
			ImGui::SliderFloat("Model Transform X", &modelVector.x, 0.0f, 1.0f, "%.1f", 1.0f);
			ImGui::Text("Model Transform X: %.1f", modelVector.x);

			ImGui::SliderFloat("Model Transform Y", &modelVector.y, 0.0f, 1.0f, "%.1f", 1.0f);
			ImGui::Text("Model Transform Y: %.1f", modelVector.y);

			ImGui::SliderFloat("Model Transform Z", &modelVector.z, 0.0f, 1.0f, "%.1f", 1.0f);
			ImGui::Text("Model Transform Z: %.1f", modelVector.z);

			// View matrix
			// Eye
			ImGui::SliderFloat("View Eye X", &viewEye.x, 0.0f, 5.0f, "%.1f", 1.0f);
			ImGui::Text("View Eye X: %.1f", viewEye.x);

			ImGui::SliderFloat("View Eye Y", &viewEye.y, 0.0f, 5.0f, "%.1f", 1.0f);
			ImGui::Text("View Eye Y: %.1f", viewEye.y);

			ImGui::SliderFloat("View Eye Z", &viewEye.z, 0.0f, 5.0f, "%.1f", 1.0f);
			ImGui::Text("View Eye Z: %.1f", viewEye.z);

			// Center
			ImGui::SliderFloat("View Center X", &viewCenter.x, 0.0f, 5.0f, "%.1f", 1.0f);
			ImGui::Text("View Center X: %.1f", viewCenter.x);
			ImGui::SliderFloat("View Center Y", &viewCenter.y, 0.0f, 5.0f, "%.1f", 1.0f);
			ImGui::Text("View Center Y: %.1f", viewCenter.y);
			ImGui::SliderFloat("View Center Z", &viewCenter.z, -1.0f, 1.0f, "%.1f", 1.0f);
			ImGui::Text("View Center Z: %.1f", viewCenter.z);

			// Projection
			ImGui::Checkbox("Show Orthographic:", &showOrthoProjection);
			if (showOrthoProjection)
			{
				// TODO: add sliders for ortho projection
				ImGui::SliderFloat("Ortho Left", &orthoLeft, -5.0f, 0.0f, "%.1f", 1.0f);
				ImGui::Text("Ortho Left: %.1f", orthoLeft);

				ImGui::SliderFloat("Ortho Right", &orthoRight, 0.0f, 5.0f, "%.1f", 1.0f);
				ImGui::Text("Ortho Right: %.1f", orthoRight);

				ImGui::SliderFloat("Ortho Bottom", &orthoBottom, -5.0f, 0.0f, "%.1f", 1.0f);
				ImGui::Text("Ortho Bottom: %.1f", orthoBottom);

				ImGui::SliderFloat("Ortho Top", &orthoTop, 0.0f, 5.0f, "%.1f", 1.0f);
				ImGui::Text("Ortho Top: %.1f", orthoTop);
			}
			else
			{
				ImGui::SliderFloat("Field of View", &fov, 0.0f, 100.0f, "%.1f", 1.0f);
			}

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