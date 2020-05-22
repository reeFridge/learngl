#ifndef ROTATION_H
#define ROTATION_H

struct Rotation;

#include <glm/glm.hpp>

struct Rotation
{
    float yaw;
    float pitch;
    float roll;
    Rotation();
    Rotation(float yat, float pitch, float roll);
    Rotation(const glm::vec3&);
    glm::vec3 to_vector();
};

#endif
