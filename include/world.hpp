#pragma once

#include "chunk.hpp"

struct WorldRayHit {
    glm::ivec3 block;
    glm::ivec3 side;
};

class World {
public:
    World() = default;
    ~World() = default;

    Chunk* createChunk(int x, int y, int z);
    Chunk* getChunk(int x, int y, int z) const;

    Chunk* getChunkContainingBlock(int x, int y, int z) const;
    const auto& getChunks() const { return _chunks; }

    unsigned char getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, unsigned char blockId);

    std::optional<WorldRayHit> findRayHit(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;

private:
    std::unordered_map<uint64_t, std::unique_ptr<Chunk>> _chunks;
};

uint64_t getChunkKey(int x, int y, int z);