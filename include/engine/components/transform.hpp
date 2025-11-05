#pragma once

#include "engine/core/core.hpp"

class Transform {
public:
    Transform()
        : position(0.0f, 0.0f, 0.0f),
          rotation(0.0f, 0.0f, 0.0f),
          scale(1.0f, 1.0f, 1.0f) {}

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 getForward() const {
        glm::vec3 forward;

        forward.x = cos(glm::radians(rotation.y - 90.0f)) * cos(glm::radians(rotation.x));
        forward.y = sin(glm::radians(rotation.x));
        forward.z = sin(glm::radians(rotation.y - 90.0f)) * cos(glm::radians(rotation.x));

        return glm::normalize(forward);
    }

    glm::vec3 getRight() const {
        return glm::normalize(glm::cross(getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::vec3 getUp() const {
        return glm::normalize(glm::cross(getRight(), getForward()));
    }

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position) *
                glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::scale(model, scale);

        return model;
    }
};