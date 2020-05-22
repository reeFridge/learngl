#ifndef CAMERA_H
#define CAMERA_H

struct Camera;

#include <glm/glm.hpp>
#include "rotation.h"

struct Camera
{
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 position;
    float speed;
    float fov;
    Rotation rotation;
    Camera(float, float, glm::vec3, Rotation);
    void update();
    glm::mat4 view_matrix();
};

#endif
