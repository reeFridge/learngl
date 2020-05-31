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
glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

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

	unsigned int vs = shader::loadFromFile("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fs = shader::loadFromFile("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int lightFs = shader::loadFromFile("./shaders/light_fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int objShader = shader::createProgram(vs, fs);
	unsigned int lightShader = shader::createProgram(vs, lightFs);
	glDeleteShader(vs);
	glDeleteShader(fs);
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

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glUseProgram(objShader);
	glUniform3f(glGetUniformLocation(objShader, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(objShader, "objColor"), 1.0f, 0.5f, 0.31f);
	float ambientStrength = 0.1f, specularStrength = 0.5f;
	glUniform1f(glGetUniformLocation(objShader, "ambientStrength"), ambientStrength);
	glUniform1f(glGetUniformLocation(objShader, "specularStrength"), specularStrength);

	while (!glfwWindowShouldClose(window.raw))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		process_input(window.raw);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0f), view(1.0f), proj;
		//model = glm::scale(model, glm::vec3(0.6f, 1.4f, 0.2f));
		view = view * camera.view_matrix();
		proj = glm::perspective(glm::radians(camera.fov), (float)(W/H), 0.1f, 100.0f);

		{
			glUseProgram(objShader);
			glUniform3fv(glGetUniformLocation(objShader, "lightPos"), 1, glm::value_ptr(lightPos));
			glUniform3fv(glGetUniformLocation(objShader, "viewPos"), 1, glm::value_ptr(camera.position));
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
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

		glfwSwapBuffers(window.raw);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteProgram(objShader);
	glDeleteProgram(lightShader);

	return 0;
}
