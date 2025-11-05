#include "engine/gfx/buffer/index_buffer.hpp"

#include <iostream>
#include <glad/gl.h>

using namespace gfx;

IndexBuffer::IndexBuffer() {
    glGenBuffers(1, &_id);
    std::cout << "Created IndexBuffer with ID: " << _id << std::endl;
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &_id);
    std::cout << "Deleted IndexBuffer with ID: " << _id << std::endl;
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setData(const void* data, uint32_t size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

    _count = size / sizeof(unsigned int);
}