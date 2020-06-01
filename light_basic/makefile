CC = gcc
CXX = g++

LEARNGL_ROOT = ..
UTILS = $(LEARNGL_ROOT)/utils

GLAD = $(LEARNGL_ROOT)/glad
GLAD_SRC = $(GLAD)/src/glad.c
GLAD_INCLUDE = ${GLAD}/include

GLM = $(LEARNGL_ROOT)/glm

STB_IMAGE = $(LEARNGL_ROOT)/stb_image

INCLUDES = -I$(GLAD_INCLUDE) -I$(STB_IMAGE) -I$(GLM) -I$(LEARNGL_ROOT)
LDLIBS = -lX11 -lglfw3 -lGL -lpthread -ldl
CFLAGS = -Wall -I$(GLAD_INCLUDE)
CXXFLAGS = -std=c++11 -Wall $(INCLUDES)

TARGET = main

cpp_files =	src/main.cpp $(wildcard $(UTILS)/*.cpp)

objects = $(cpp_files:.cpp=.o) glad.o

all: $(TARGET)

$(TARGET): $(objects)
	$(CXX) -o $@ $^ $(LDLIBS)

glad.o: $(GLAD_SRC)
	$(CC) $(CFLAGS) -c -o glad.o $(GLAD_SRC)

clean:
	rm $(TARGET) $(objects)
