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

VertexBuffer::VertexBuffer(const BufferLayout& layout) : _layout(layout) {
    glGenBuffers(1, &_id);
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
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}