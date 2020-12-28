#include <cstdio>
#include "window.h"

void size_callback(GLFWwindow* window, int width, int height);

Window::Window(unsigned int width, unsigned int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    this->raw = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (this->raw == NULL)
	return;
    glfwMakeContextCurrent(this->raw);
    glfwSetFramebufferSizeCallback(this->raw, size_callback);
    glfwSetInputMode(this->raw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Window::~Window()
{
    printf("Window destroyed\n");
    glfwTerminate();	
}

void size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
