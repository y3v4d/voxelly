#pragma once

#include "engine/core/core.hpp"

class Transform2D {
public:
    Transform2D(const glm::vec2& position = glm::vec2(0.0f, 0.0f), float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f, 1.0f), const glm::vec2& anchor = glm::vec2(0.5f, 0.5f))
        : position(position),
          rotation(rotation),
          size(size),
          anchor(anchor) {}

    glm::vec2 position;
    float rotation; // degrees
    glm::vec2 size;
    glm::vec2 anchor;
};