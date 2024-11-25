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

float vertices[48] = {
	//     position    |          color
	-0.5f, -0.5f, 0.0f,		0.3f, 0.0f, 0.4f, // front - bottom left
	0.5f, -0.5f, 0.0f,		0.0f, 0.1f, 0.5f, // front - bottom right
	0.5f, 0.5f, 0.0f,		0.3f, 0.2f, 0.0f, // front - top right
	-0.5, 0.5f, 0.0f,		0.3f, 0.0f, 0.4f, // front - top left
	-0.5f, -0.5f, 0.5f,		0.0f, 0.1f, 0.5f, // back - bottom left
	0.5f, -0.5f, 0.5f,		0.3f, 0.2f, 0.0f, // back - bottom right
	0.5f, 0.5f, 0.5f,		0.3f, 0.0f, 0.4f, // back - top right
	-0.5f, 0.5f, 0.5f,		0.0f, 0.1f, 0.5f, // back - top left
};

unsigned int indices[36] = {
	0, 1, 2, 0, 2, 3, // front face
	4, 0, 3, 4, 7, 3, // left face
	4, 5, 6, 4, 7, 6, // back face
	1, 5, 6, 1, 2, 6, // right face
	2, 3, 6, 3, 7, 6, // top face
	0, 1, 5, 1, 4, 5, // bottom face
};

float prismVertices[24] = {
	//		position      |      colors
	-0.3f, -0.6f, 0.0f,		1.0f, 0.0f, 0.0f, // front - bottom left
	0.3f, -0.6f, 0.0f,		0.0f, 1.0f, 0.0f, // front - bottom right
	0.0f, 0.6f, 0.0f,		0.0f, 0.0f, 1.0f, // front - top
	0.0f, -0.6f, -0.5f,		0.5f, 0.0f, 0.5f, // back
};

unsigned int prismIndices[12] = {
	0, 1, 2, // front face
	0, 1, 3, // bottom face
	0, 2, 3, // left face
	1, 2, 3, // right face
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
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// Prism
	unsigned int VAO2, VBO2, EBO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prismVertices), &prismVertices, GL_STATIC_DRAW);
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prismIndices), &prismIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	ShaderProgram shaderProgram = ShaderProgram(
		"resources/vertex.shader",
		"resources/fragment.shader"
	);

	// ---- MVP
	// Model
	glm::vec3 modelVector = glm::vec3(0.0f, 0.0f, 0.0f);
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
	bool useViewMatrix = false;
	bool useProjectionMatrix = false;

	// TODO: add color to vertices
	// TODO: add texture

	unsigned int mvpLocation = glGetUniformLocation(shaderProgram.getProgramId(), "modelViewProjection");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glBindVertexArray(VAO);
		glClearColor(0.1f, 0.1f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();

		// Send MVP to shader with uniform
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), modelVector);
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		glm::mat4 modelViewProjection;

		if (useViewMatrix)
		{
			viewMatrix = glm::lookAt(
				viewEye, // eye
				viewCenter, // center
				viewUpDown // up/down - use y only?
			);
		}

		if (useProjectionMatrix)
		{
			if (showOrthoProjection)
			{
				glm::mat4 orthoProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, 0.1f, 100.0f);
				if (useViewMatrix) modelViewProjection = orthoProjection * viewMatrix * modelMatrix;
				else modelViewProjection = orthoProjection * modelMatrix;
			}
			else
			{
				glm::mat4 perspectiveProjection = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);
				if (useViewMatrix) modelViewProjection = perspectiveProjection * viewMatrix * modelMatrix;
				else modelViewProjection = perspectiveProjection * modelMatrix;
			}
		} else modelViewProjection = modelMatrix;

		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &modelViewProjection[0][0]);

		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, (void*)0);

		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, sizeof(prismIndices), GL_UNSIGNED_INT, (void*)0);

		// Dear Im GUI
		ImGui::SetNextWindowSize(windowSize);
		bool imGuiwindow = ImGui::Begin("Im GUI Hello");
		if (imGuiwindow)
		{
			// Model Matrix
			ImGui::Text("Model Matrix");
			ImGui::SliderFloat("Model Transform X", &modelVector.x, -1.0f, 1.0f, "%.1f", 1.0f);
			ImGui::SliderFloat("Model Transform Y", &modelVector.y, -1.0f, 1.0f, "%.1f", 1.0f);
			ImGui::SliderFloat("Model Transform Z", &modelVector.z, -1.0f, 1.0f, "%.1f", 1.0f);

			// View matrix
			ImGui::Text("View Matrix");
			ImGui::Checkbox("Use View Matrix", &useViewMatrix);
			if (useViewMatrix)
			{
				// Eye
				ImGui::SliderFloat("View Eye X", &viewEye.x, -5.0f, 5.0f, "%.1f", 1.0f);
				ImGui::SliderFloat("View Eye Y", &viewEye.y, -5.0f, 5.0f, "%.1f", 1.0f);
				ImGui::SliderFloat("View Eye Z", &viewEye.z, -5.0f, 5.0f, "%.1f", 1.0f);

				// Center
				ImGui::SliderFloat("View Center X", &viewCenter.x, 0.0f, 5.0f, "%.1f", 1.0f);
				ImGui::SliderFloat("View Center Y", &viewCenter.y, 0.0f, 5.0f, "%.1f", 1.0f);
				ImGui::SliderFloat("View Center Z", &viewCenter.z, -1.0f, 1.0f, "%.1f", 1.0f);
			}

			// Projection
			ImGui::Text("Projection Matrix");
			ImGui::Checkbox("Use Projection Matrix", &useProjectionMatrix);
			if (useProjectionMatrix)
			{
				ImGui::Checkbox("Show Orthographic:", &showOrthoProjection);
				if (showOrthoProjection)
				{
					// TODO: add sliders for ortho projection
					ImGui::SliderFloat("Ortho Left", &orthoLeft, -5.0f, 0.0f, "%.1f", 1.0f);
					ImGui::SliderFloat("Ortho Right", &orthoRight, 0.0f, 5.0f, "%.1f", 1.0f);
					ImGui::SliderFloat("Ortho Bottom", &orthoBottom, -5.0f, 0.0f, "%.1f", 1.0f);
					ImGui::SliderFloat("Ortho Top", &orthoTop, 0.0f, 5.0f, "%.1f", 1.0f);
				}
				else
				{
					ImGui::SliderFloat("Field of View", &fov, 0.0f, 100.0f, "%.1f", 1.0f);
				}
			}

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