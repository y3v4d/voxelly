#pragma once

#include "engine/core/core.hpp"

namespace assets {
    class Image {
    public:
        Image() : _width(0), _height(0), _channels(0), _data(nullptr) {}

        Image(const std::string& path);

        Image(unsigned int width, unsigned int height, unsigned int channels)
            : _width(width), _height(height), _channels(channels) {
            _data = new unsigned char[width * height * channels];
            std::fill(_data, _data + (width * height * channels), 0);
        }

        Image(unsigned int w, unsigned int h, unsigned int c, unsigned char* d)
            : _width(w), _height(h), _channels(c), _data(d) {}

        ~Image();

        void resize(unsigned int width, unsigned int height);
        void setPixel(unsigned int x, unsigned int y, const glm::vec4& color);

        glm::vec4 getPixel(unsigned int x, unsigned int y, glm::vec4& outColor) const;
        glm::vec4 getPixel(unsigned int x, unsigned int y) const;

        const unsigned char* getData() const { return _data; }
        
        int getWidth() const { return _width; }
        int getHeight() const { return _height; }
        int getChannels() const { return _channels; }

    private:
        int _width, _height, _channels;
        unsigned char* _data;
    };
}