#pragma once

#include <memory>
#include "engine/assets/image.hpp"

namespace gfx {
    class Texture {
    public:
        Texture();
        Texture(const unsigned char* data, int width, int height, int channels);
        Texture(const assets::Image& image);
        ~Texture();

        void uploadImage(const unsigned char* data, int width, int height, int channels);

        unsigned int getID() const { return _id; }

    private:
        unsigned int _id;
    };
}