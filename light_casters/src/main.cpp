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
#include <utils/texture.h>
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
	unsigned int phongDirVS = shader::loadFromFile("./shaders/phong_dir_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int phongDirFS = shader::loadFromFile("./shaders/phong_dir_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int phongSpotVS = shader::loadFromFile("./shaders/phong_spot_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int phongSpotFS = shader::loadFromFile("./shaders/phong_spot_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int lightFs = shader::loadFromFile("./shaders/light_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int objPhongShader = shader::createProgram(phongvs, phongfs);
	unsigned int objPhongDirShader = shader::createProgram(phongDirVS, phongDirFS);
	unsigned int objPhongSpotShader = shader::createProgram(phongSpotVS, phongSpotFS);
	unsigned int lightShader = shader::createProgram(phongvs, lightFs);
	glDeleteShader(phongvs);
	glDeleteShader(phongfs);
	glDeleteShader(phongDirVS);
	glDeleteShader(phongDirFS);
	glDeleteShader(phongSpotVS);
	glDeleteShader(phongSpotFS);
	glDeleteShader(lightShader);
	
	unsigned int container_tex = texture::loadTexture("./textures/container2.png");
	unsigned int emission_map = texture::loadTexture("./textures/matrix.jpg");
	unsigned int container_tex_specular = texture::loadTexture("./textures/container2_specular.png");
	
	unsigned int VBO, VAO, lightVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figures::cube_with_normals_and_tex_coords), figures::cube_with_normals_and_tex_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	ImVec4 clearColor = ImVec4(0.2, 0.2, 0.2, 1.0f);
	ImVec4 lightAmbient = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 lightDiffuse = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 lightSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 lightDirection = ImVec4(-0.2f, -1.0f, -0.3f, 1.0f);
	ImVec4 spotDirection = ImVec4(0.0f, 0.0f, -1.0f, 1.0f);
	const char* items[] = { "2", "4", "8", "16", "32", "64", "128", "256" };
	int current = 4;

	float cutoffAngle = 12.5f;
	float attenuationLinear = 0.09f;
	float attenuationQuadratic = 0.032f;
	bool shouldRotate = false;

	ImVec4 lightPos = ImVec4(-1.0f, 0.0f, 5.0f, 1.0f);
	enum LightCasterType { Directional, Point, Spot };
	const char* lightCasterTypeNames[3] = { "Directional", "Point", "Spot" };
	int currentLightCasterType = Spot;
    glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	while (!glfwWindowShouldClose(window.raw))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		process_input(window.raw);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container_tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, container_tex_specular);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emission_map);

		glm::mat4 view(1.0f), proj;
		view = view * camera.view_matrix();
		proj = glm::perspective(glm::radians(camera.fov), (float)(W/H), 0.1f, 100.0f);

		{
			unsigned int objShader = objPhongShader;
			if (currentLightCasterType == Directional)
			{
				objShader = objPhongDirShader;
			}
			if (currentLightCasterType == Point)
			{
				objShader = objPhongShader;
			}
			if (currentLightCasterType == Spot)
			{
				objShader = objPhongSpotShader;
			}
			glUseProgram(objShader);

			glUniform1i(glGetUniformLocation(objShader, "material.diffuse"), 0);
			glUniform1i(glGetUniformLocation(objShader, "material.specular"), 1);
			glUniform1i(glGetUniformLocation(objShader, "material.emission"), 2);
			glUniform1ui(glGetUniformLocation(objShader, "material.shininess"), atoi(items[current]));
			glUniform3f(glGetUniformLocation(objShader, "light.ambient"), lightAmbient.x, lightAmbient.y, lightAmbient.z);
			glUniform3f(glGetUniformLocation(objShader, "light.diffuse"), lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
			glUniform3f(glGetUniformLocation(objShader, "light.specular"), lightSpecular.x, lightSpecular.y, lightSpecular.z);
			if (currentLightCasterType == Directional)
			{
				glUniform3f(glGetUniformLocation(objShader, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);
			}
			else
			{
				glUniform3f(glGetUniformLocation(objShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			}

			if (currentLightCasterType == Point || currentLightCasterType == Spot)
			{
				glUniform1f(glGetUniformLocation(objShader, "light.constant"), 1.0f);
				glUniform1f(glGetUniformLocation(objShader, "light.linear"), attenuationLinear);
				glUniform1f(glGetUniformLocation(objShader, "light.quadratic"), attenuationQuadratic);
			}

			if (currentLightCasterType == Spot)
			{
				glUniform3f(glGetUniformLocation(objShader, "spotDir"), spotDirection.x, spotDirection.y, spotDirection.z);
				glUniform1f(glGetUniformLocation(objShader, "light.cutoffAngle"), glm::cos(glm::radians(cutoffAngle)));
			}

			glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(objShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

			glBindVertexArray(VAO);

			for (unsigned int i = 0; i < 10; ++i)
			{
				glm::mat4 model(1.0f);
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(view * model));
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				if (shouldRotate)
				{
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				}
				glUniformMatrix3fv(glGetUniformLocation(objShader, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
				glUniformMatrix4fv(glGetUniformLocation(objShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		if (currentLightCasterType != Directional)
		{
			glUseProgram(lightShader);
			glm::mat4 model(1.0f);
			model = glm::translate(model, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
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
		//ImGui::ShowDemoWindow();

		ImGui::Begin("Controls");
		ImGui::ColorEdit3("clear color", (float*)&clearColor);
		ImGui::ColorEdit3("ambient light color", (float*)&lightAmbient);
		ImGui::ColorEdit3("diffuse light color", (float*)&lightDiffuse);
		ImGui::ColorEdit3("specular light color", (float*)&lightSpecular);
		ImGui::Combo("shininess", &current, items, IM_ARRAYSIZE(items));
		ImGui::Combo("light caster", &currentLightCasterType, lightCasterTypeNames, IM_ARRAYSIZE(lightCasterTypeNames));
		ImGui::Checkbox("rotate boxes", (bool*)&shouldRotate);
		if (currentLightCasterType == Point || currentLightCasterType == Spot)
		{
			ImGui::SliderFloat("linear term", &attenuationLinear, 0.027f, 0.7f);
			ImGui::SliderFloat("quadratic term", &attenuationQuadratic, 0.003f, 1.8f);
			ImGui::InputFloat3("light pos", (float*)&lightPos);
		}
		if (currentLightCasterType == Directional)
		{
			ImGui::InputFloat3("light direction", (float*)&lightDirection);
		}
		if (currentLightCasterType == Spot)
		{
			ImGui::InputFloat3("spot direction", (float*)&spotDirection);
			ImGui::SliderFloat("spot cutoff angle", &cutoffAngle, 0.0f, 25.0f);
		}
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
	glDeleteProgram(objPhongDirShader);
	glDeleteProgram(lightShader);
	glDeleteProgram(objPhongSpotShader);
	glDeleteTextures(1, &container_tex);
	glDeleteTextures(1, &container_tex_specular);
	glDeleteTextures(1, &emission_map);

	return 0;
}
