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
	25.0f, // speed
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

	unsigned int vs = shader::loadFromFile("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fs = shader::loadFromFile("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int sp = shader::createProgram(vs, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(figures::cube), figures::cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(sp);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window.raw))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		process_input(window.raw);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glm::mat4 model(1.0f), view(1.0f), proj;
		view = view * camera.view_matrix();
		proj = glm::perspective(glm::radians(camera.fov), (float)(W/H), 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(sp, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(sp, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(sp, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(sp, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window.raw);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(sp);

	return 0;
}
