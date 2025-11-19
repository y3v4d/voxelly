#include "engine/assets/image.hpp"
#include "engine/core/filesystem.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace assets;

Image::Image(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);

    auto buffer = core::FileSystem::readFromFile(path);
    _data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(buffer.data()), 
        static_cast<int>(buffer.size()), 
        &_width, &_height, &_channels, 
        0
    );
}

Image::~Image() {
    if (_data) {
        stbi_image_free(_data);
    }
}

void Image::resize(unsigned int width, unsigned int height) {
    if (_data) {
        free(_data);
    }

    _width = width;
    _height = height;

    _data = new unsigned char[width * height * _channels];
    std::fill(_data, _data + (width * height * _channels), 0);
}

void Image::setPixel(unsigned int x, unsigned int y, const glm::vec4& color) {
    if (x >= static_cast<unsigned int>(_width) || y >= static_cast<unsigned int>(_height) || !_data) {
        return;
    }

    unsigned char* pixel = _data + (y * _width + x) * _channels;

    pixel[0] = static_cast<unsigned char>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
    pixel[1] = static_cast<unsigned char>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
    pixel[2] = static_cast<unsigned char>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);

    if (_channels >= 4) {
        pixel[3] = static_cast<unsigned char>(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f);
    }
}

glm::vec4 Image::getPixel(unsigned int x, unsigned int y) const {
    glm::vec4 color;
    return getPixel(x, y, color);
}

glm::vec4 Image::getPixel(unsigned int x, unsigned int y, glm::vec4& outColor) const {
    if (x >= static_cast<unsigned int>(_width) || y >= static_cast<unsigned int>(_height) || !_data) {
        outColor = glm::vec4(0.0f);
        return outColor;
    }

    const unsigned char* pixel = _data + (y * _width + x) * _channels;

    outColor.r = pixel[0] / 255.0f;
    outColor.g = pixel[1] / 255.0f;
    outColor.b = pixel[2] / 255.0f;

    if (_channels >= 4) {
        outColor.a = pixel[3] / 255.0f;
    } else {
        outColor.a = 1.0f;
    }

    return outColor;
}