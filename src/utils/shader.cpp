#include "shader.h"
#include <cstdio>
#include <cstdlib>

char* readFile(const char *path)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
	printf("Error: Can't open file '%s'.\n", path);
	return NULL;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(sizeof(char) * (len + 1));
    buffer[len] = '\0';
    fread(buffer, sizeof(char), len, file);
    fclose(file);
    return buffer;
}

namespace shader
{
    void load(unsigned int* ref, const char* source)
    {
	glShaderSource(*ref, 1, &source, NULL);
	glCompileShader(*ref);

	int success;
	char log[512];
	glGetShaderiv(*ref, GL_COMPILE_STATUS, &success);

	if (!success)
	{
	    glGetShaderInfoLog(*ref, 512, NULL, log);
	    printf("Error: shader compilation failed\n%s\n", log);
	}
    }

    void link(unsigned int* program, unsigned int* vertex, unsigned int* fragment)
    {
	glAttachShader(*program, *vertex);
	glAttachShader(*program, *fragment);
	glLinkProgram(*program);

	int success;
	char log[512];
	glGetProgramiv(*program, GL_LINK_STATUS, &success);
	if (!success)
	{
	    glGetProgramInfoLog(*program, 512, NULL, log);
	    printf("Error: program compilation failed\n%s\n", log);
	}
    }

    unsigned int loadFromFile(const char* path, GLenum type)
    {
	const char* source = readFile(path);
	unsigned int shader = glCreateShader(type);
	load(&shader, source);
	free((void*)source);
	return shader;
    }

    unsigned int createProgram(unsigned int vertex, unsigned int fragment)
    {
	unsigned int program = glCreateProgram();
	link(&program, &vertex, &fragment);
	return program;
    }
}
