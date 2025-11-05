#include "engine/assets/image.hpp"
#include "engine/core/filesystem.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace assets;

Image::Image(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);

    auto buffer = core::FileSystem::readFromFile(path);
    data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(buffer.data()), 
        static_cast<int>(buffer.size()), 
        &width, &height, &channels, 
        0
    );
}

Image::~Image() {
    if (data) {
        stbi_image_free(data);
    }
}