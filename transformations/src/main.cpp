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

const unsigned int W = 800;
const unsigned int H = 600;

int main()
{


    Window window(W, H);
    if (window.raw == NULL)
    {
	printf("Failed to create GLFW window\n");
	return -1;
    }

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

    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

    while (!glfwWindowShouldClose(window.raw))
    {
	window.process_input();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crate_tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, c_tex);

	glUniform1f(glGetUniformLocation(shaderProgram, "mix_f"), 0.5f);

	glm::mat4 trans(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5, 0.5, 0.0));
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glm::mat4 trans_other(1.0f);
	float factor = sin((float)glfwGetTime());
	trans_other = glm::translate(trans_other, glm::vec3(-0.5, -0.5, 0.0));
	trans_other = glm::scale(trans_other, glm::vec3(factor, factor, 1.0));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans_other));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	window.swap_buffers();
	glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &crate_tex);
    glDeleteTextures(1, &c_tex);

    return 0;
}
