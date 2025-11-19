#include "engine/assets/file.hpp"

using namespace assets;

FileAsset::~FileAsset() {
    if(_data) {
        delete[] _data;
    }
}

void FileAsset::loadFromBuffer(const char* data, size_t size) {
    if(_data) {
        delete[] _data;

        _data = nullptr;
        _size = 0;
    }

    _data = new char[size];
    _size = size;

    std::memcpy(_data, data, size);
}