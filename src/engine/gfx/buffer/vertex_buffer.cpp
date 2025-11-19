#include "engine/gfx/buffer/vertex_buffer.hpp"

#include <cassert>
#include <iostream>
#include <glad/gl.h>

using namespace gfx;

uint32_t gfx::getBufferDataTypeSize(BufferDataType type) {
    switch (type) {
        case BufferDataType::FLOAT:   return 4;
        case BufferDataType::FLOAT2:  return 4 * 2;
        case BufferDataType::FLOAT3:  return 4 * 3;
        case BufferDataType::FLOAT4:  return 4 * 4;
        case BufferDataType::INT:     return 4;
        case BufferDataType::INT2:    return 4 * 2;
        case BufferDataType::INT3:    return 4 * 3;
        case BufferDataType::INT4:    return 4 * 4;
        case BufferDataType::MAT3:    return 4 * 3 * 3;
        case BufferDataType::MAT4:    return 4 * 4 * 4;
        case BufferDataType::BOOL:    return 1;
        case BufferDataType::NONE:    return 0;
    }

    assert(false && "Unknown BufferDataType!");
}

uint32_t gfx::getBufferDataTypeCount(BufferDataType type) {
    switch (type) {
        case BufferDataType::FLOAT:   return 1;
        case BufferDataType::FLOAT2:  return 2;
        case BufferDataType::FLOAT3:  return 3;
        case BufferDataType::FLOAT4:  return 4;
        case BufferDataType::INT:     return 1;
        case BufferDataType::INT2:    return 2;
        case BufferDataType::INT3:    return 3;
        case BufferDataType::INT4:    return 4;
        case BufferDataType::MAT3:    return 3 * 3;
        case BufferDataType::MAT4:    return 4 * 4;
        case BufferDataType::BOOL:    return 1;
        case BufferDataType::NONE:    return 0;
    }

    assert(false && "Unknown BufferDataType!");
}

GLenum bufferUsageToGLenum(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::STATIC: return GL_STATIC_DRAW;
        case BufferUsage::DYNAMIC: return GL_DYNAMIC_DRAW;
        case BufferUsage::STREAM: return GL_STREAM_DRAW;
    }

    assert(false && "Unknown BufferUsage!");
}

BufferLayout::BufferLayout(std::vector<BufferLayoutElement> elements)
    : _elements(elements), _stride(0) {
    for (auto& element : _elements) {
        element.offset = _stride;
        _stride += getBufferDataTypeSize(element.type);
    }


}

BufferLayout::BufferLayout(std::initializer_list<BufferLayoutElement> elements)
    : _elements(elements), _stride(0) {
    for (auto& element : _elements) {
        element.offset = _stride;
        _stride += getBufferDataTypeSize(element.type);
    }
}

VertexBuffer::VertexBuffer(const BufferLayout& layout, BufferUsage usage) : _layout(layout), _usage(usage) {
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, _layout.getStride(), nullptr, bufferUsageToGLenum(_usage));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::cout << "Created VertexBuffer with ID: " << _id << std::endl;
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &_id);
    std::cout << "Deleted VertexBuffer with ID: " << _id << std::endl;
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void* data, uint32_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, size, data, bufferUsageToGLenum(_usage));
}

void VertexBuffer::setSubData(const void* data, uint32_t size, uint32_t offset) {
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::setUsage(BufferUsage usage) {
    _usage = usage;

    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, _layout.getStride(), nullptr, bufferUsageToGLenum(_usage));
}