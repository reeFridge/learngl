#ifndef WINDOW_H
#define WINDOW_H

struct Window;

#include <GLFW/glfw3.h>

struct Window
{
  GLFWwindow* raw;
  Window(unsigned int, unsigned int);
  ~Window();
};

#endif
