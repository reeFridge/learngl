#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utils/window.h>
#include <utils/shader.h>
#include <utils/texture.h>
#include <utils/rotation.h>
#include <utils/camera.h>

const unsigned int W = 800;
const unsigned int H = 600;

Camera camera(45.0f, 25.0f, glm::vec3(0.0f, 0.0f, 10.0f), Rotation(90.0f, 0.0f, 0.0f));

float lastTime = 0.0f;
float deltaTime = 0.0f;

float last_mouse_x = 400, last_mouse_y = 300;

bool first_mouse_enter = true;

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (first_mouse_enter) {
		first_mouse_enter = false;
		last_mouse_x = xpos;
		last_mouse_y = ypos;
	}

	float diff_x = xpos - last_mouse_x, diff_y = ypos - last_mouse_y;
	last_mouse_x = xpos;
	last_mouse_y = ypos;
	const float mouse_sense = 0.05f;
	diff_x *= mouse_sense;
	diff_y *= mouse_sense;

	camera.rotation.yaw += diff_x;
	camera.rotation.pitch += diff_y;

	if (camera.rotation.pitch > 89.0f)
		camera.rotation.pitch = 89.0f;
	if (camera.rotation.pitch < -89.0f)
		camera.rotation.pitch = -89.0f;

	camera.update();
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
}

int main()
{
	Window window(W, H);
	if (window.raw == NULL)
	{
		printf("Failed to create GLFW window\n");
		return -1;
	}

	glfwSetCursorPosCallback(window.raw, mouse_callback);
	glfwSetScrollCallback(window.raw, mouse_scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to init GLAD\n");
		return -1;
	}

	unsigned int vertexShader = shader::loadFromFile("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = shader::loadFromFile("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	unsigned int shaderProgram = shader::createProgram(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int crate_tex = texture::loadFromFile("../resources/textures/crate.jpg", GL_RGB, true, GL_REPEAT, GL_NEAREST);
	unsigned int c_tex = texture::loadFromFile("../resources/textures/c-logo.png", GL_RGBA, true, GL_REPEAT, GL_NEAREST);

	float cube_vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "t0"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "t1"), 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	glm::vec3 positions[] = {
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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, crate_tex);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, c_tex);

		glUniform1f(glGetUniformLocation(shaderProgram, "mix_f"), 0.5f);

		glBindVertexArray(VAO);

		glm::mat4 model(1.0f), view(1.0f), proj;
		view = view * camera.view_matrix();
		proj = glm::perspective(glm::radians(camera.fov), (float)(W/H), 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

		for (int i = 0; i < 10; i++) {
			glm::mat4 transform(1.0f);
			transform = glm::translate(transform, positions[i]);
			float angle = 20.0f * i;
			transform = glm::rotate(transform, (glm::radians(angle) + (float)glfwGetTime()), glm::vec3(1.0f, 0.3, 0.5)) * deltaTime;
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window.raw);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteProgram(shaderProgram);
	glDeleteTextures(1, &crate_tex);
	glDeleteTextures(1, &c_tex);

	return 0;
}
