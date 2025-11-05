#include "engine/assets/file.hpp"

#include <fstream>

using namespace assets;

FileAsset::FileAsset(const std::string& path) : path(path) {
    data = nullptr;
    size = 0;
    
    load();
}

FileAsset::~FileAsset() {
    if(data) {
        delete[] data;
    }
}

void FileAsset::load() {
    if(data) {
        delete[] data;
        data = nullptr;
        size = 0;
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    size = file.tellg();
    data = new char[size];

    file.seekg(0, std::ios::beg);
    file.read(data, size);

    file.close();
}