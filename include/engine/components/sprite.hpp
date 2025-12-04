#pragma once

#include "engine/core/core.hpp"
#include "engine/assets/image.hpp"
#include "engine/components/transform2d.hpp"
#include "engine/gfx/texture.hpp"

class Sprite : public Transform2D {
public:
    Sprite() : _texture(nullptr), _uvs(0.0f, 0.0f, 1.0f, 1.0f), _color(1.0f, 1.0f, 1.0f, 1.0f) {}
    Sprite(shared<gfx::Texture> texture, const glm::vec4& uvs = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        : _texture(texture), _uvs(uvs), _color(color) {}

    void setTexture(shared<gfx::Texture> texture) {
        _texture = texture;
    }

    void setUVs(const glm::vec4& uvs) {
        _uvs = uvs;
    }

    void setColor(const glm::vec4& color) {
        _color = color;
    }

    const shared<gfx::Texture>& getTexture() const {
        return _texture;
    }

    const glm::vec4& getUVs() const {
        return _uvs;
    }

    const glm::vec4& getColor() const {
        return _color;
    }

private:
    shared<gfx::Texture> _texture;
    glm::vec4 _uvs;
    glm::vec4 _color;
};