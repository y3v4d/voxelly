#pragma once

#include "engine/core/core.hpp"

namespace assets {
    class Image {
    public:
        Image() : width(0), height(0), channels(0), data(nullptr) {}

        Image(const std::string& path);
        Image(unsigned int w, unsigned int h, unsigned int c, unsigned char* d)
            : width(w), height(h), channels(c), data(d) {}
        ~Image();

        int width, height, channels;
        unsigned char* data;
    };
}