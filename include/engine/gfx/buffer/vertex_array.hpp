#pragma once

#include <cstdint>

#include "./vertex_buffer.hpp"
#include "./index_buffer.hpp"

namespace gfx {
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;
        void unbind() const;

        void setVertexBuffer(std::unique_ptr<VertexBuffer> vertexBuffer);
        void setIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer);

        constexpr uint32_t getID() const { return _id; }

        const std::unique_ptr<VertexBuffer>& getVertexBuffer() const {
            return _vertexBuffer;
        }

        const std::unique_ptr<IndexBuffer>& getIndexBuffer() const {
            return _indexBuffer;
        }

    private:
        uint32_t _id;
        
        std::unique_ptr<VertexBuffer> _vertexBuffer;
        std::unique_ptr<IndexBuffer> _indexBuffer;
    };
}