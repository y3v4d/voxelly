#include "chunk_mesh.hpp"
#include "world.hpp"

#define CHUNK_MAX_VERTICES (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 4)
#define CHUNK_MAX_INDICES (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6)

const float FACE_VERTICES[] = {
    // FRONT FACE
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,

    // BACK FACE
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,

    // LEFT FACE
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,

    // RIGHT FACE
    1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,

    // TOP FACE
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    // BOTTOM FACE
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

const float FACE_TEXCOORDS[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

const unsigned int FACE_INDICES[] = {
    0, 1, 2,
    2, 3, 0,
};

const float FACE_NORMALS[] = {
    // FRONT FACE
    0.0f, 0.0f, 1.0f,
    // BACK FACE
    0.0f, 0.0f, -1.0f,
    // LEFT FACE
    -1.0f, 0.0f, 0.0f,
    // RIGHT FACE
    1.0f, 0.0f, 0.0f,
    // TOP FACE
    0.0f, 1.0f, 0.0f,
    // BOTTOM FACE
    0.0f, -1.0f, 0.0f
};

void emitFace(
    float* vertices, unsigned int* indices,
    unsigned int& vertex_count, unsigned int& index_count,
    int face, float textureID, float fx, float fy, float fz
) {
    for (int i = 0; i < 4; ++i) {
        vertices[vertex_count * 7 + 0] = FACE_VERTICES[(face * 4 + i) * 3 + 0] + fx;
        vertices[vertex_count * 7 + 1] = FACE_VERTICES[(face * 4 + i) * 3 + 1] + fy;
        vertices[vertex_count * 7 + 2] = FACE_VERTICES[(face * 4 + i) * 3 + 2] + fz;
        vertices[vertex_count * 7 + 3] = textureID;
        vertices[vertex_count * 7 + 4] = FACE_NORMALS[(face) * 3 + 0];
        vertices[vertex_count * 7 + 5] = FACE_NORMALS[(face) * 3 + 1];
        vertices[vertex_count * 7 + 6] = FACE_NORMALS[(face) * 3 + 2];

        vertex_count++;
    }

    indices[index_count++] = FACE_INDICES[0] + vertex_count - 4;
    indices[index_count++] = FACE_INDICES[1] + vertex_count - 4;
    indices[index_count++] = FACE_INDICES[2] + vertex_count - 4;
    indices[index_count++] = FACE_INDICES[3] + vertex_count - 4;
    indices[index_count++] = FACE_INDICES[4] + vertex_count - 4;
    indices[index_count++] = FACE_INDICES[5] + vertex_count - 4;
}

std::unique_ptr<gfx::VertexArray> createCubeMeshVAO(int textureID) {
    auto vao = std::make_unique<gfx::VertexArray>();

    auto vbo = std::make_unique<gfx::VertexBuffer>(
        gfx::BufferLayout({
            { gfx::BufferDataType::FLOAT3 },
            { gfx::BufferDataType::FLOAT },
            { gfx::BufferDataType::FLOAT3 }
        })
    );

    auto ibo = std::make_unique<gfx::IndexBuffer>();

    vao->setVertexBuffer(std::move(vbo));
    vao->setIndexBuffer(std::move(ibo));

    renderCubeMesh(*vao, textureID);

    return vao;
}

void renderCubeMesh(gfx::VertexArray& vao, int textureID) {
    float vertices[6 * 4 * 7];
    unsigned int indices[6 * 6];

    unsigned int vertex_count = 0;
    unsigned int index_count = 0;

    for (int face = 0; face < 6; ++face) {
        emitFace(vertices, indices, vertex_count, index_count, face, static_cast<float>(textureID), 0.0f, 0.0f, 0.0f);
    }

    vao.getVertexBuffer()->setData(vertices, vertex_count * 7 * sizeof(float));
    vao.getIndexBuffer()->setData(indices, index_count * sizeof(unsigned int));
}

ChunkMesh::ChunkMesh(Chunk* chunk)
    : _chunk(chunk) {
    _vertex_buffer = new float[CHUNK_MAX_VERTICES * 7]; // 7 floats per vertex (position, normal, texcoord)
    _index_buffer = new unsigned int[CHUNK_MAX_INDICES];

    auto vbo = std::make_unique<gfx::VertexBuffer>(
        gfx::BufferLayout({
            { gfx::BufferDataType::FLOAT3 },
            { gfx::BufferDataType::FLOAT },
            { gfx::BufferDataType::FLOAT3 }
        })
    );

    auto ibo = std::make_unique<gfx::IndexBuffer>();

    _vao.setVertexBuffer(std::move(vbo));
    _vao.setIndexBuffer(std::move(ibo));
}

ChunkMesh::~ChunkMesh() {
    delete[] _vertex_buffer;
    delete[] _index_buffer;
}

void ChunkMesh::updateMesh() const {
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;

    auto world = _chunk->getWorld();
    int x = _chunk->getX();
    int y = _chunk->getY();
    int z = _chunk->getZ();

    std::array<Chunk*, 6> neighborChunks {
        world->getChunk(x, y, z + 1), // FRONT
        world->getChunk(x, y, z - 1), // BACK
        world->getChunk(x - 1, y, z), // LEFT
        world->getChunk(x + 1, y, z), // RIGHT
        world->getChunk(x, y + 1, z), // TOP
        world->getChunk(x, y - 1, z)  // BOTTOM
    };

    auto getBlock = [&](int bx, int by, int bz) -> unsigned char {
        if(bx < 0) {
            if(!neighborChunks[2]) return 0;
            return neighborChunks[2]->getBlock(bx + CHUNK_SIZE, by, bz);
        } else if(bx >= CHUNK_SIZE) {
            if(!neighborChunks[3]) return 0;
            return neighborChunks[3]->getBlock(bx - CHUNK_SIZE, by, bz);
        }

        if(by < 0) {
            if(!neighborChunks[5]) return 0;
            return neighborChunks[5]->getBlock(bx, by + CHUNK_SIZE, bz);
        } else if(by >= CHUNK_SIZE) {
            if(!neighborChunks[4]) return 0;
            return neighborChunks[4]->getBlock(bx, by - CHUNK_SIZE, bz);
        }

        if(bz < 0) {
            if(!neighborChunks[1]) return 0;
            return neighborChunks[1]->getBlock(bx, by, bz + CHUNK_SIZE);
        } else if(bz >= CHUNK_SIZE) {
            if(!neighborChunks[0]) return 0;
            return neighborChunks[0]->getBlock(bx, by, bz - CHUNK_SIZE);
        }

        return _chunk->getBlock(bx, by, bz);
    };

    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                auto blockID = _chunk->getBlock(x, y, z);
                if(!blockID) {
                    continue;
                }

                float textureID = blockID - 1;

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                // FRONT FACE
                if(!getBlock(x, y, z + 1)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 0, textureID, fx, fy, fz);
                }

                // BACK FACE
                if(!getBlock(x, y, z - 1)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 1, textureID, fx, fy, fz);
                }

                // LEFT FACE
                if(!getBlock(x - 1, y, z)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 2, textureID, fx, fy, fz);
                }

                // RIGHT FACE
                if(!getBlock(x + 1, y, z)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 3, textureID, fx, fy, fz);
                }

                // TOP FACE
                if(!getBlock(x, y + 1, z)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 4, textureID, fx, fy, fz);
                }

                // BOTTOM FACE
                if(!getBlock(x, y - 1, z)) {
                    emitFace(_vertex_buffer, _index_buffer, vertex_count, index_count, 5, textureID, fx, fy, fz);
                }
            }
        }
    }

    _vao.getVertexBuffer()->setData(_vertex_buffer, vertex_count * 8 * sizeof(float));
    _vao.getIndexBuffer()->setData(_index_buffer, index_count * sizeof(unsigned int));
}