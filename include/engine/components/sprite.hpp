#pragma once

#include "engine/core/core.hpp"
#include "engine/assets/image.hpp"
#include "engine/components/transform.hpp"

class Sprite : public Transform {
public:
    Sprite() : _image(nullptr) {}
    Sprite(shared<assets::Image> image) : _image(image) {}

    void setImage(shared<assets::Image> image) {
        _image = image;
    }

    shared<assets::Image> getImage() const {
        return _image;
    }
private:
    shared<assets::Image> _image;
};