#pragma once

#include <cstddef>
#include <string>

namespace assets {
    class Asset {
    public:
        virtual ~Asset() = default;

        virtual void loadFromBuffer(const char* data, size_t size) = 0;

        std::string id;
    };
}