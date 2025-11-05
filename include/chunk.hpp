#pragma once

#include <array>
#include <glm/glm.hpp>

#include "engine/engine.hpp"

#define CHUNK_SIZE 16

class World;
class Chunk {
public:
    Chunk(int x, int y, int z, World* world);
    ~Chunk();

    unsigned char getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, unsigned char type);

    void setDirty(bool dirty) { _dirty = dirty; }
    bool isDirty() const { return _dirty; }

    World* getWorld() const { return _world; }
    
    int getX() const { return _x; }
    int getY() const { return _y; }
    int getZ() const { return _z; }
    
private:
    std::array<unsigned char, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> _data;
    
    int _x, _y, _z;
    World* _world;

    bool _dirty = true;
};