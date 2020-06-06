#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utils/window.h>
#include <utils/shader.h>
#include <utils/rotation.h>
#include <utils/camera.h>
#include <common/figures.h>

const unsigned int W = 800;
const unsigned int H = 600;

Camera camera(
	45.0f, // fov
	5.0f, // speed
	glm::vec3(0.0f, 0.0f, 10.0f),
	Rotation(90.0f, 0.0f, 0.0f)
);

float lastTime = 0.0f, deltaTime = 0.0f;

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float updated_fov = camera.fov - yoffset;
	if (updated_fov >= 1.0f && updated_fov <= 45.0f)
		camera.fov = updated_fov;
	else if (updated_fov <= 1.0f)
		camera.fov = 1.0f;
	else if (updated_fov >= 45.0f)
		camera.fov = 45.0f;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 z = camera.direction * camera.speed * deltaTime;
	glm::vec3 x = camera.right * camera.speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.position -= z;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.position += z;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.position -= x;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.position += x;
/*	
	float lightSpeed = 2.0f;
	glm::vec3 lightZ = glm::vec3(0.0f, 0.0f, 1.0f) * lightSpeed * deltaTime;
	glm::vec3 lightX = glm::vec3(1.0f, 0.0f, 0.0f) * lightSpeed * deltaTime;
	glm::vec3 lightY = glm::vec3(0.0f, 1.0f, 0.0f) * lightSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPos -= lightZ;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPos += lightZ;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		lightPos -= lightX;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos += lightX;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos -= lightY;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos += lightY;
*/
	const float rotation_speed = 2.0f;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.rotation.pitch -= rotation_speed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.rotation.pitch += rotation_speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.rotation.yaw -= rotation_speed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.rotation.yaw += rotation_speed;
	
	if (camera.rotation.pitch > 89.0f)
		camera.rotation.pitch = 89.0f;
	if (camera.rotation.pitch < -89.0f)
		camera.rotation.pitch = -89.0f;

	camera.update();
}

int main()
{
	Window window(W, H);
	if (window.raw == NULL)
	{
		printf("Failed to create GLFW window\n");
		return -1;
	}

	glfwSetScrollCallback(window.raw, mouse_scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to init GLAD\n");
		return -1;
	}

	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window.raw, true);
	const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

	unsigned int phongvs = shader::loadFromFile("./shaders/phong_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int phongfs = shader::loadFromFile("./shaders/phong_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int gouraudvs = shader::loadFromFile("./shaders/gouraud_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int gouraudfs = shader::loadFromFile("./shaders/gouraud_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int lightFs = shader::loadFromFile("./shaders/light_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int objPhongShader = shader::createProgram(phongvs, phongfs);
	unsigned int objGouraudShader = shader::createProgram(gouraudvs, gouraudfs);
	unsigned int lightShader = shader::createProgram(phongvs, lightFs);
	glDeleteShader(phongvs);
	glDeleteShader(phongfs);
	glDeleteShader(gouraudvs);
	glDeleteShader(gouraudfs);
	glDeleteShader(lightShader);
	
	unsigned int VBO, VAO, lightVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figures::cube_with_normals), figures::cube_with_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	float ambientStrength = 0.1f, specularStrength = 0.5f;
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);
	ImVec4 lightColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 objColor = ImVec4(1.0f, 0.5f, 0.31f, 1.0f);
	const char* items[] = { "2", "4", "8", "16", "32", "64", "128", "256" };
	int current = 4;

	float lightRadius = 2.0f;
	float lightAngle = 0.0f;

	glm::vec3 lightPos(0.0f);
	enum ShadingMethod { Gouraud, Phong };
	const char* methodNames[2] = { "Gouraud", "Phong" };
	int currentMethod = Phong;
	
	while (!glfwWindowShouldClose(window.raw))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		process_input(window.raw);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0f), view(1.0f), proj;
		view = view * camera.view_matrix();
		proj = glm::perspective(glm::radians(camera.fov), (float)(W/H), 0.1f, 100.0f);

		lightPos.x = cos(lightAngle);
		lightPos.z = sin(lightAngle);
		lightPos *= lightRadius;

		{
			unsigned int objShader = objPhongShader;
			if (currentMethod == Phong)
				glUseProgram(objPhongShader);
			else if (currentMethod == Gouraud) {
				objShader = objGouraudShader;
				glUseProgram(objGouraudShader);
			}

			glUniform1ui(glGetUniformLocation(objShader, "shiness"), atoi(items[current]));
			glUniform1f(glGetUniformLocation(objShader, "ambientStrength"), ambientStrength);
			glUniform1f(glGetUniformLocation(objShader, "specularStrength"), specularStrength);
			glUniform3f(glGetUniformLocation(objShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(objShader, "objColor"), objColor.x, objColor.y, objColor.z);
			glUniform3fv(glGetUniformLocation(objShader, "lightPos"), 1, glm::value_ptr(lightPos));
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(view * model));
			glUniformMatrix3fv(glGetUniformLocation(objShader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
			glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(objShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		{
			glUseProgram(lightShader);
			glm::mat4 model(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
//		ImGui::ShowDemoWindow();

		ImGui::Begin("Controls");
		const char* methodName = methodNames[currentMethod];
		ImGui::SliderInt("shading method", &currentMethod, 0, 1, methodName);
		ImGui::ColorEdit3("clear color", (float*)&clearColor);
		ImGui::ColorEdit3("light color", (float*)&lightColor);
		ImGui::ColorEdit3("obj color", (float*)&objColor);
		ImGui::Combo("shiness", &current, items, IM_ARRAYSIZE(items));
		ImGui::SliderFloat("specular strength", &specularStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("ambient strength", &ambientStrength , 0.0f, 1.0f);
		ImGui::SliderAngle("light angle", &lightAngle);
		ImGui::SliderFloat("light radius", &lightRadius, 1.0f, 5.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window.raw);
		glfwPollEvents();
	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteProgram(objPhongShader);
	glDeleteProgram(objGouraudShader);
	glDeleteProgram(lightShader);

	return 0;
}
