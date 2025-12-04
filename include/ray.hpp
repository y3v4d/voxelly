#pragma once

#include <glm/glm.hpp>

class Ray {
public:
    Ray() : origin(glm::vec3(0.0f)), direction(glm::vec3(0.0f, 0.0f, -1.0f)) {}
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin(origin), direction(glm::normalize(direction)) {}

    glm::vec3 origin;
    glm::vec3 direction;

    void at(float t, glm::vec3& outPoint) const {
        outPoint = origin + direction * t;
    }
};