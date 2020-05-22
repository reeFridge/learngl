#include "rotation.h"

Rotation::Rotation()
{
    this->yaw = 0.0f;
    this->pitch = 0.0f;
    this->roll = 0.0f;
}

Rotation::Rotation(float yaw, float pitch, float roll)
{
    this->yaw = yaw;
    this->pitch = pitch;
    this->roll = roll;
}

Rotation::Rotation(const glm::vec3& direction)
{
    this->pitch = glm::degrees(asin(direction.y));
    this->yaw = glm::degrees(acos((direction.x / cos(glm::radians(this->pitch)))));
    this->roll = 0.0f;
}

glm::vec3 Rotation::to_vector()
{
    glm::vec3 vector(0.0f, 0.0f, 0.0f);
    vector.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    vector.y = sin(glm::radians(this->pitch));
    vector.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    return glm::normalize(vector);
}
