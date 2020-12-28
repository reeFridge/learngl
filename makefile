CC = gcc
CXX = g++

SRC_ROOT = ./src
UTILS = $(SRC_ROOT)/utils

LIB_ROOT = ./lib
GLAD = $(LIB_ROOT)/glad
GLAD_SRC = $(GLAD)/src/glad.c
GLAD_INCLUDE = ${GLAD}/include

GLM = $(LIB_ROOT)/glm
IMGUI = $(LIB_ROOT)/imgui
IMGUI_EXAMPLES = $(IMGUI)/examples

STB_IMAGE = $(LIB_ROOT)/stb_image

INCLUDES = -I$(GLAD_INCLUDE) -I$(STB_IMAGE) -I$(GLM) -I$(SRC_ROOT) -I$(IMGUI) -I$(IMGUI_EXAMPLES)
LDLIBS = -lX11 -lglfw -lGL -lpthread -ldl -lassimp
CFLAGS = -Wall -I$(GLAD_INCLUDE)
CXXFLAGS = -std=c++11 -Wall -DIMGUI_IMPL_OPENGL_LOADER_GLAD $(INCLUDES)

SOURCES = $(SRC_ROOT)/main.cpp $(wildcard $(UTILS)/*.cpp)
SOURCES += $(IMGUI)/examples/imgui_impl_glfw.cpp $(IMGUI)/examples/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI)/imgui.cpp $(IMGUI)/imgui_demo.cpp $(IMGUI)/imgui_draw.cpp $(IMGUI)/imgui_widgets.cpp

GLAD_OBJ = $(BUILD_ROOT)/glad.o
OBJ_FILES = $(SOURCES:%.cpp=$(BUILD_ROOT)/%.o)
BUILD_ROOT = ./build

TARGET_EXEC = $(BUILD_ROOT)/main

build: $(TARGET_EXEC)

clean:
	rm -rf $(BUILD_ROOT)

.PHONY: build clean

$(TARGET_EXEC): $(OBJ_FILES) $(GLAD_OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

$(OBJ_FILES): $(BUILD_ROOT)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(GLAD_OBJ): $(GLAD_SRC)
	$(CC) $(CFLAGS) -c -o $(GLAD_OBJ) $(GLAD_SRC)
