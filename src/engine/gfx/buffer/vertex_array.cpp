#include "engine/gfx/buffer/vertex_array.hpp"

#ifdef __EMSCRIPTEN__
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif

using namespace gfx;

static uint32_t getGLType(BufferDataType type) {
    switch (type) {
        case BufferDataType::FLOAT:   return GL_FLOAT;
        case BufferDataType::FLOAT2:  return GL_FLOAT;
        case BufferDataType::FLOAT3:  return GL_FLOAT;
        case BufferDataType::FLOAT4:  return GL_FLOAT;
        case BufferDataType::INT:     return GL_INT;
        case BufferDataType::INT2:    return GL_INT;
        case BufferDataType::INT3:    return GL_INT;
        case BufferDataType::INT4:    return GL_INT;
        case BufferDataType::MAT3:    return GL_FLOAT;
        case BufferDataType::MAT4:    return GL_FLOAT;
        case BufferDataType::BOOL:    return GL_BOOL;
        case BufferDataType::NONE:    return 0;
    }

    return 0;
}

VertexArray::VertexArray() {
    glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &_id);
}

void VertexArray::bind() const {
    glBindVertexArray(_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::setVertexBuffer(std::unique_ptr<VertexBuffer> vertexBuffer) {
    bind();
    vertexBuffer->bind();

    const auto& layout = vertexBuffer->getLayout();
    const auto& elements = layout.getElements();
    uint32_t stride = layout.getStride();

    for (uint32_t i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            getBufferDataTypeCount(element.type),
            getGLType(element.type),
            element.normalized ? GL_TRUE : GL_FALSE,
            stride,
            reinterpret_cast<const void*>(element.offset)
        );
    }

    _vertexBuffer = std::move(vertexBuffer);
    unbind();
}

void VertexArray::setIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) {
    bind();

    indexBuffer->bind();
    _indexBuffer = std::move(indexBuffer);
    
    unbind();
}