#pragma once

#include <cstdint>
#include <vector>

namespace gfx {
    enum class BufferDataType {
        NONE = 0,
        FLOAT, FLOAT2, FLOAT3, FLOAT4,
        INT, INT2, INT3, INT4,
        MAT3, MAT4,
        BOOL
    };

    enum class BufferUsage {
        STATIC = 0,
        DYNAMIC,
        STREAM
    };

    struct BufferLayoutElement {
        BufferLayoutElement(BufferDataType type)
            : type(type), offset(0), normalized(false) {}

        BufferDataType type;
        bool normalized;

        uint32_t offset;
    };

    class BufferLayout {
    public:
        BufferLayout() = default;
        BufferLayout(std::vector<BufferLayoutElement> elements);
        BufferLayout(std::initializer_list<BufferLayoutElement> elements);
        
        const std::vector<BufferLayoutElement>& getElements() const {
            return _elements;
        }

        uint32_t getStride() const {
            return _stride;
        }

    private:
        std::vector<BufferLayoutElement> _elements;
        uint32_t _stride = 0;
    };

    class VertexBuffer {
    public:
        VertexBuffer() = delete;
        VertexBuffer(const BufferLayout& layout, BufferUsage usage = BufferUsage::STATIC);
        
        ~VertexBuffer();

        void bind() const;
        void unbind() const;

        void setData(const void* data, uint32_t size);
        void setSubData(const void* data, uint32_t size, uint32_t offset);

        void setUsage(BufferUsage usage);
        BufferUsage getUsage() const { return _usage; }

        constexpr uint32_t getID() const { return _id; }
        const BufferLayout& getLayout() const { return _layout; }

    private:
        uint32_t _id;
        BufferLayout _layout;
        BufferUsage _usage;
    };

    uint32_t getBufferDataTypeSize(BufferDataType type);
    uint32_t getBufferDataTypeCount(BufferDataType type);
}