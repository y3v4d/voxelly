#include "chunk.hpp"

Chunk::Chunk(int x, int y, int z, World* world) {
    _data.fill(0);

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

    int index = z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x;
    if(_data[index] == type) {
        return;
    }
    
    _data[index] = type;
    if(type == 0) {
        _blockCount = std::max(0, _blockCount - 1);
    } else {
        _blockCount++;
    }
    _dirty = true;
}