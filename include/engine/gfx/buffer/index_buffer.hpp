#pragma once

#include <cstdint>

namespace gfx {
    class IndexBuffer {
    public:
        IndexBuffer();
        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        void setData(const void* data, uint32_t size);
        
        constexpr uint32_t getID() const { return _id; }
        constexpr uint32_t getCount() const { return _count; }

    private:
        uint32_t _id;
        uint32_t _count;
    };
}