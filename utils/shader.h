#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

char* readFile(const char*);

namespace shader
{
    void load(unsigned int*, const char*);
    void link(unsigned int*, unsigned int*, unsigned int*);
    unsigned int loadFromFile(const char*, GLenum type);
    unsigned int createProgram(unsigned int vertex, unsigned int fragment);
}
#endif
