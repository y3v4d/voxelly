#pragma once

#include <string>

#include "engine/assets/asset.hpp"

namespace assets {
    class FileAsset : public Asset {
    public:
        ~FileAsset();

        virtual void loadFromBuffer(const char* data, size_t size) override;
        
        const char* getData() const { return _data; }
        size_t getSize() const { return _size; }
    private:
        char* _data;
        size_t _size;
    };
}