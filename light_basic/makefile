CC = gcc
CXX = g++

LEARNGL_ROOT = ..
UTILS = $(LEARNGL_ROOT)/utils

GLAD = $(LEARNGL_ROOT)/glad
GLAD_SRC = $(GLAD)/src/glad.c
GLAD_INCLUDE = ${GLAD}/include

GLM = $(LEARNGL_ROOT)/glm
IMGUI = $(LEARNGL_ROOT)/imgui
IMGUI_EXAMPLES = $(IMGUI)/examples

STB_IMAGE = $(LEARNGL_ROOT)/stb_image

INCLUDES = -I$(GLAD_INCLUDE) -I$(STB_IMAGE) -I$(GLM) -I$(LEARNGL_ROOT) -I$(IMGUI) -I$(IMGUI_EXAMPLES)
LDLIBS = -lX11 -lglfw3 -lGL -lpthread -ldl
CFLAGS = -Wall -I$(GLAD_INCLUDE)
CXXFLAGS = -std=c++11 -Wall -DIMGUI_IMPL_OPENGL_LOADER_GLAD $(INCLUDES)

TARGET = main

SOURCES =	src/main.cpp $(wildcard $(UTILS)/*.cpp)
SOURCES += $(IMGUI)/examples/imgui_impl_glfw.cpp $(IMGUI)/examples/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI)/imgui.cpp $(IMGUI)/imgui_demo.cpp $(IMGUI)/imgui_draw.cpp $(IMGUI)/imgui_widgets.cpp

objects = $(SOURCES:.cpp=.o) glad.o

all: $(TARGET)

$(TARGET): $(objects)
	$(CXX) -o $@ $^ $(LDLIBS)

glad.o: $(GLAD_SRC)
	$(CC) $(CFLAGS) -c -o glad.o $(GLAD_SRC)

clean:
	rm $(TARGET) $(objects)
