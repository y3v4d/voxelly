#pragma once

#include <array>
#include <glm/glm.hpp>

#include "engine/engine.hpp"

#define CHUNK_SIZE 16
// macro function to perform 3D DDA algorithm
#define PERFORM_DDA(origin, direction, maxDistance, action) { \
    glm::ivec3 voxel( \
        static_cast<int>(std::floor(origin.x)), \
        static_cast<int>(std::floor(origin.y)), \
        static_cast<int>(std::floor(origin.z)) \
    ); \
    glm::ivec3 side(0, 0, 0); \
    \
    int stepX = (direction.x > 0) ? 1 : -1; \
    int stepY = (direction.y > 0) ? 1 : -1; \
    int stepZ = (direction.z > 0) ? 1 : -1; \
    \
    auto intbound = [](float s, float ds) { \
        if(ds > 0) { \
            return (glm::ceil(s) - s) / ds; \
        } else if(ds < 0) { \
            return (s - glm::floor(s)) / -ds; \
        } else { \
            return std::numeric_limits<float>::infinity(); \
        } \
    }; \
    \
    float tMaxX = intbound(origin.x, direction.x); \
    float tMaxY = intbound(origin.y, direction.y); \
    float tMaxZ = intbound(origin.z, direction.z); \
    \
    float tDeltaX = direction.x != 0 ? static_cast<float>(stepX) / direction.x : std::numeric_limits<float>::infinity(); \
    float tDeltaY = direction.y != 0 ? static_cast<float>(stepY) / direction.y : std::numeric_limits<float>::infinity(); \
    float tDeltaZ = direction.z != 0 ? static_cast<float>(stepZ) / direction.z : std::numeric_limits<float>::infinity(); \
    \
    float distance = 0.0f; \
    \
    while(distance < maxDistance) { \
        if((voxel.x < 0 && stepX < 0) || \
           (voxel.y < 0 && stepY < 0) || \
           (voxel.z < 0 && stepZ < 0)) { \
            break; \
        } \
        \
        action \
        \
        if(tMaxX < tMaxY && tMaxX < tMaxZ) { \
            voxel.x += stepX; \
            distance = tMaxX; \
            tMaxX += tDeltaX; \
            side = glm::ivec3(-stepX, 0, 0); \
        } else if(tMaxY < tMaxZ) { \
            voxel.y += stepY; \
            distance = tMaxY; \
            tMaxY += tDeltaY; \
            side = glm::ivec3(0, -stepY, 0); \
        } else { \
            voxel.z += stepZ; \
            distance = tMaxZ; \
            tMaxZ += tDeltaZ; \
            side = glm::ivec3(0, 0, -stepZ); \
        } \
    } \
}

class World;
class Chunk {
public:
    Chunk(int x, int y, int z, World* world);
    ~Chunk();

    unsigned char getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, unsigned char type);

    const std::array<unsigned char, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>& getBlocks() const {
        return _data;
    }

    void setDirty(bool dirty) { _dirty = dirty; }
    bool isDirty() const { return _dirty; }

    World* getWorld() const { return _world; }
    
    int getX() const { return _x; }
    int getY() const { return _y; }
    int getZ() const { return _z; }

    int getBlockCount() const { return _blockCount; }
    
private:
    std::array<unsigned char, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> _data;
    
    int _x, _y, _z;
    World* _world;

    int _blockCount = 0;
    bool _dirty = true;
};