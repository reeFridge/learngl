#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float speed, glm::vec3 position, Rotation rotation)
{
    this->rotation = rotation;
    this->fov = fov;
    this->speed = speed;
    this->position = position;
    this->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    this->update();
}

void Camera::update()
{
    this->direction = this->rotation.to_vector();
    this->right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0f, 0.0), this->direction));
    this->up = glm::normalize(glm::cross(this->direction, this->right));
}

glm::mat4 Camera::view_matrix()
{
    glm::mat4 transform_matrix(1.0f);
    transform_matrix[0] = glm::vec4(this->right, 0.0f);
    transform_matrix[1] = glm::vec4(this->up, 0.0f);
    transform_matrix[2] = glm::vec4(this->direction, 0.0f);

    return glm::transpose(transform_matrix) * glm::translate(glm::mat4(1.0f), -(this->position));
}
