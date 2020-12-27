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

const unsigned int W = 1024;
const unsigned int H = 768;

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

	unsigned int combinedVS = shader::loadFromFile("./shaders/phong_combined_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int combinedFS = shader::loadFromFile("./shaders/phong_combined_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int phongvs = shader::loadFromFile("./shaders/phong_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int lightFS = shader::loadFromFile("./shaders/light_fragment.glsl", GL_FRAGMENT_SHADER);

	unsigned int objPhongShader = shader::createProgram(combinedVS, combinedFS);
	unsigned int lightShader = shader::createProgram(phongvs, lightFS);

	glDeleteShader(phongvs);
	glDeleteShader(combinedVS);
	glDeleteShader(combinedFS);
	glDeleteShader(lightFS);
	
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

	// directionalLight
	ImVec4 directionalLightAmbient = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 directionalLightDiffuse = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 directionalLightSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 directionalLightDir = ImVec4(-0.2f, -1.0f, -0.3f, 1.0f);

	// pointLight
	ImVec4 pointLightAmbient = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 pointLightDiffuse = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
	ImVec4 pointLightSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 pointLightPos = ImVec4(0.0f, 2.0f, -2.0f, 1.0f);

	// spotLight
	ImVec4 spotLightAmbient = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 spotLightDiffuse = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
	ImVec4 spotLightSpecular = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 spotLightPos = ImVec4(0.0f, 0.0f, 2.0f, 1.0f);
	ImVec4 spotLightDir = ImVec4(0.0f, 0.0f, -1.0f, 1.0f);

	const char* items[] = { "2", "4", "8", "16", "32", "64", "128", "256" };
	int current = 4;

	float cutoffAngle = 12.5f;
	float outerCutoffAngle = 17.5f;
	float attenuationLinear = 0.09f;
	float attenuationQuadratic = 0.032f;
	bool shouldRotate = false;

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
			glUseProgram(objShader);

			glUniform1i(glGetUniformLocation(objShader, "material.diffuse"), 0);
			glUniform1i(glGetUniformLocation(objShader, "material.specular"), 1);
			glUniform1i(glGetUniformLocation(objShader, "material.emission"), 2);
			glUniform1ui(glGetUniformLocation(objShader, "material.shininess"), atoi(items[current]));

			glUniform3f(glGetUniformLocation(objShader, "directionalLight.ambient"), directionalLightAmbient.x, directionalLightAmbient.y, directionalLightAmbient.z);
			glUniform3f(glGetUniformLocation(objShader, "directionalLight.diffuse"), directionalLightDiffuse.x, directionalLightDiffuse.y, directionalLightDiffuse.z);
			glUniform3f(glGetUniformLocation(objShader, "directionalLight.specular"), directionalLightSpecular.x, directionalLightSpecular.y, directionalLightSpecular.z);
			glUniform3f(glGetUniformLocation(objShader, "directionalLightDir"), directionalLightDir.x, directionalLightDir.y, directionalLightDir.z);

			glUniform3f(glGetUniformLocation(objShader, "pointLight.ambient"), pointLightAmbient.x, pointLightAmbient.y, pointLightAmbient.z);
			glUniform3f(glGetUniformLocation(objShader, "pointLight.diffuse"), pointLightDiffuse.x, pointLightDiffuse.y, pointLightDiffuse.z);
			glUniform3f(glGetUniformLocation(objShader, "pointLight.specular"), pointLightSpecular.x, pointLightSpecular.y, pointLightSpecular.z);
			glUniform3f(glGetUniformLocation(objShader, "pointLightPos"), pointLightPos.x, pointLightPos.y, pointLightPos.z);
			glUniform1f(glGetUniformLocation(objShader, "pointLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(objShader, "pointLight.linear"), attenuationLinear);
			glUniform1f(glGetUniformLocation(objShader, "pointLight.quadratic"), attenuationQuadratic);

			glUniform3f(glGetUniformLocation(objShader, "spotLight.ambient"), spotLightAmbient.x, spotLightAmbient.y, spotLightAmbient.z);
			glUniform3f(glGetUniformLocation(objShader, "spotLight.diffuse"), spotLightDiffuse.x, spotLightDiffuse.y, spotLightDiffuse.z);
			glUniform3f(glGetUniformLocation(objShader, "spotLight.specular"), spotLightSpecular.x, spotLightSpecular.y, spotLightSpecular.z);
			glUniform3f(glGetUniformLocation(objShader, "spotLightPos"), spotLightPos.x, spotLightPos.y, spotLightPos.z);
			glUniform3f(glGetUniformLocation(objShader, "spotLightDir"), spotLightDir.x, spotLightDir.y, spotLightDir.z);
			glUniform1f(glGetUniformLocation(objShader, "spotLight.cutoffAngle"), glm::cos(glm::radians(cutoffAngle)));
			glUniform1f(glGetUniformLocation(objShader, "spotLight.outerCutoffAngle"), glm::cos(glm::radians(outerCutoffAngle)));
			glUniform1f(glGetUniformLocation(objShader, "spotLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(objShader, "spotLight.linear"), attenuationLinear);
			glUniform1f(glGetUniformLocation(objShader, "spotLight.quadratic"), attenuationQuadratic);

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

		// draw light positions
		glUseProgram(lightShader);
		glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 2; ++i)
		{
			glm::mat4 model(1.0f);

			if (i == 0)
			{
				model = glm::translate(model, glm::vec3(pointLightPos.x, pointLightPos.y, pointLightPos.z));
				glUniform3f(glGetUniformLocation(lightShader, "color"), pointLightDiffuse.x, pointLightDiffuse.y, pointLightDiffuse.z);
			}
			else
			{
				model = glm::translate(model, glm::vec3(spotLightPos.x, spotLightPos.y, spotLightPos.z));
				glUniform3f(glGetUniformLocation(lightShader, "color"), spotLightDiffuse.x, spotLightDiffuse.y, spotLightDiffuse.z);
			}
			
			model = glm::scale(model, glm::vec3(0.2f));

			glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();

		ImGui::Begin("Controls");
		ImGui::ColorEdit3("clear color", (float*)&clearColor);
		ImGui::Combo("shininess", &current, items, IM_ARRAYSIZE(items));
		ImGui::Checkbox("rotate boxes", (bool*)&shouldRotate);

		if (ImGui::CollapsingHeader("DirectionalLight"))
		{
			ImGui::ColorEdit3("dir ambient color", (float*)&directionalLightAmbient);
			ImGui::ColorEdit3("dir diffuse color", (float*)&directionalLightDiffuse);
			ImGui::ColorEdit3("dir specular color", (float*)&directionalLightSpecular);
			ImGui::InputFloat3("dir direction", (float*)&directionalLightDir);
		}

		ImGui::SliderFloat("linear term", &attenuationLinear, 0.027f, 0.7f);
		ImGui::SliderFloat("quadratic term", &attenuationQuadratic, 0.003f, 1.8f);

		if (ImGui::CollapsingHeader("PointLight"))
		{
			ImGui::ColorEdit3("point ambient color", (float*)&pointLightAmbient);
			ImGui::ColorEdit3("point diffuse color", (float*)&pointLightDiffuse);
			ImGui::ColorEdit3("point specular color", (float*)&pointLightSpecular);
			ImGui::InputFloat3("point position", (float*)&pointLightPos);
		}

		if (ImGui::CollapsingHeader("SpotLight"))
		{
			ImGui::ColorEdit3("spot ambient color", (float*)&spotLightAmbient);
			ImGui::ColorEdit3("spot diffuse color", (float*)&spotLightDiffuse);
			ImGui::ColorEdit3("spot specular color", (float*)&spotLightSpecular);
			ImGui::InputFloat3("spot position", (float*)&spotLightPos);
			ImGui::InputFloat3("spot direction", (float*)&spotLightDir);
			ImGui::SliderFloat("spot inner cutoff angle", &cutoffAngle, 0.0f, 25.0f);
			ImGui::SliderFloat("spot outer cutoff angle", &outerCutoffAngle, 5.0f, 25.0f);
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
	glDeleteProgram(lightShader);
	glDeleteTextures(1, &container_tex);
	glDeleteTextures(1, &container_tex_specular);
	glDeleteTextures(1, &emission_map);

	return 0;
}
