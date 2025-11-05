#include "world.hpp"

uint64_t getChunkKey(int x, int y, int z) {
    return (static_cast<uint64_t>(x) & 0xFFFFF) << 42 |
           (static_cast<uint64_t>(y) & 0xFFFFF) << 21 |
           (static_cast<uint64_t>(z) & 0xFFFFF);
}

Chunk* World::createChunk(int x, int y, int z) {
    auto key = getChunkKey(x, y, z);
    auto chunk = std::make_unique<Chunk>(x, y, z, this);
    _chunks[key] = std::move(chunk);

    return _chunks[key].get();
}

Chunk* World::getChunk(int x, int y, int z) const {
    auto key = getChunkKey(x, y, z);
    auto it = _chunks.find(key);
    if (it != _chunks.end()) {
        return it->second.get();
    }

    return nullptr;
}

Chunk* World::getChunkContainingBlock(int x, int y, int z) const {
    int chunkX = std::floor((float)x / CHUNK_SIZE);
    int chunkY = std::floor((float)y / CHUNK_SIZE);
    int chunkZ = std::floor((float)z / CHUNK_SIZE);

    return getChunk(chunkX, chunkY, chunkZ);
}

unsigned char World::getBlock(int x, int y, int z) const {
    const Chunk* chunk = getChunkContainingBlock(x, y, z);
    if (!chunk) {
        return 0;
    }

    int localX = x % CHUNK_SIZE;
    int localY = y % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;

    return chunk->getBlock(localX, localY, localZ);
}

void World::setBlock(int x, int y, int z, unsigned char blockId) {
    Chunk* chunk = getChunkContainingBlock(x, y, z);
    if (!chunk) {
        return;
    }

    int localX = x % CHUNK_SIZE;
    int localY = y % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;

    chunk->setBlock(localX, localY, localZ, blockId);
}

std::optional<WorldRayHit> World::findRayHit(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
    glm::ivec3 voxel(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );
    glm::ivec3 side(0, 0, 0);

    int stepX = (direction.x > 0) ? 1 : -1;
    int stepY = (direction.y > 0) ? 1 : -1;
    int stepZ = (direction.z > 0) ? 1 : -1;

    auto intbound = [](float s, float ds) {
        if(ds > 0) {
            return (glm::ceil(s) - s) / ds;
        } else if(ds < 0) {
            return (s - glm::floor(s)) / -ds;
        } else {
            return std::numeric_limits<float>::infinity();
        }
    };

    float tMaxX = intbound(origin.x, direction.x);
    float tMaxY = intbound(origin.y, direction.y);
    float tMaxZ = intbound(origin.z, direction.z);

    float tDeltaX = direction.x != 0 ? static_cast<float>(stepX) / direction.x : std::numeric_limits<float>::infinity();
    float tDeltaY = direction.y != 0 ? static_cast<float>(stepY) / direction.y : std::numeric_limits<float>::infinity();
    float tDeltaZ = direction.z != 0 ? static_cast<float>(stepZ) / direction.z : std::numeric_limits<float>::infinity();

    float distance = 0.0f;

    while(distance < maxDistance) {
        if(getBlock(voxel.x, voxel.y, voxel.z) != 0) {
            WorldRayHit hit;

            hit.block = voxel;
            hit.side = side;

            return hit;
        }

        if(tMaxX < tMaxY && tMaxX < tMaxZ) {
            voxel.x += stepX;
            distance = tMaxX;
            tMaxX += tDeltaX;
            side = glm::ivec3(-stepX, 0, 0);
        } else if(tMaxY < tMaxZ) {
            voxel.y += stepY;
            distance = tMaxY;
            tMaxY += tDeltaY;
            side = glm::ivec3(0, -stepY, 0);
        } else {
            voxel.z += stepZ;
            distance = tMaxZ;
            tMaxZ += tDeltaZ;
            side = glm::ivec3(0, 0, -stepZ);
        }
    }

    return std::nullopt;
}