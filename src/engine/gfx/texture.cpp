#include "engine/gfx/texture.hpp"

#include <glad/gl.h>

using namespace gfx;

Texture::Texture() {
    glGenTextures(1, &_id);
}

Texture::Texture(const unsigned char* data, int width, int height, int channels) : Texture() {
    uploadImage(data, width, height, channels);
}

Texture::Texture(const assets::Image& image)
    : Texture(image.getData(), image.getWidth(), image.getHeight(), image.getChannels()) {}

Texture::~Texture() {
    glDeleteTextures(1, &_id);
}

void Texture::uploadImage(const unsigned char* data, int width, int height, int channels) {
    GLenum format;
    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    } else {
        throw std::runtime_error("Unsupported number of channels in texture");
    }

    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}