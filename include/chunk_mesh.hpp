#pragma once

#include "chunk.hpp"

class ChunkMesh {
public:
    ChunkMesh(Chunk* chunk);
    ~ChunkMesh();

    void updateMesh() const;
    const gfx::VertexArray& getVertexArray() const { return _vao; }
    Chunk* getChunk() const { return _chunk; }

private:
    Chunk* _chunk;
    gfx::VertexArray _vao;

    float* _vertex_buffer;
    unsigned int* _index_buffer;
};

std::unique_ptr<gfx::VertexArray> createCubeMeshVAO(int textureID);
void renderCubeMesh(gfx::VertexArray& vao, int textureID);