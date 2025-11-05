#include "chunk.hpp"

Chunk::Chunk(int x, int y, int z, World* world) {
    _data.fill(0);
    for(int y = 0; y < 1; ++y) {
        for(int z = 0; z < CHUNK_SIZE; ++z) {
            for(int x = 0; x < CHUNK_SIZE; ++x) {
                setBlock(x, y, z, 1);
            }
        }
    }

    _x = x;
    _y = y;
    _z = z;
    _world = world;
}

Chunk::~Chunk() {
}

unsigned char Chunk::getBlock(int x, int y, int z) const {
    if(x < 0 || x >= CHUNK_SIZE ||
       y < 0 || y >= CHUNK_SIZE ||
       z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }

    return _data[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x];
}

void Chunk::setBlock(int x, int y, int z, unsigned char type) {
    if(x < 0 || x >= CHUNK_SIZE ||
       y < 0 || y >= CHUNK_SIZE ||
       z < 0 || z >= CHUNK_SIZE) {
        return;
    }

    _data[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x] = type;
    _dirty = true;
}