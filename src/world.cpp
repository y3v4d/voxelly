#include "world.hpp"
#include "world_asset.hpp"

#include <iostream>

uint64_t getChunkKey(int x, int y, int z) {
    return (static_cast<uint64_t>(x) & 0xFFFFF) << 42 |
           (static_cast<uint64_t>(y) & 0xFFFFF) << 21 |
           (static_cast<uint64_t>(z) & 0xFFFFF);
}

Chunk* World::createChunk(int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0) {
        return nullptr;
    }

    auto key = getChunkKey(x, y, z);
    auto chunk = std::make_unique<Chunk>(x, y, z, this);
    _chunks[key] = std::move(chunk);

    return _chunks[key].get();
}

void World::removeChunk(int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0) {
        return;
    }

    auto key = getChunkKey(x, y, z);
    _chunks.erase(key);
}

void World::removeAllChunks() {
    _chunks.clear();
}

Chunk* World::getChunk(int x, int y, int z) const {
    if(x < 0 || y < 0 || z < 0) {
        return nullptr;
    }

    auto key = getChunkKey(x, y, z);
    auto it = _chunks.find(key);
    if (it != _chunks.end()) {
        return it->second.get();
    }

    return nullptr;
}

Chunk* World::getChunkContainingBlock(int x, int y, int z) const {
    if(x < 0 || y < 0 || z < 0) {
        return nullptr;
    }

    int chunkX = std::floor((float)x / CHUNK_SIZE);
    int chunkY = std::floor((float)y / CHUNK_SIZE);
    int chunkZ = std::floor((float)z / CHUNK_SIZE);

    return getChunk(chunkX, chunkY, chunkZ);
}

unsigned char World::getBlock(int x, int y, int z) const {
    if(x < 0 || y < 0 || z < 0) {
        return 0;
    }

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
    if(x < 0 || y < 0 || z < 0) {
        return;
    }

    Chunk* chunk = getChunkContainingBlock(x, y, z);
    if (!chunk) {
        chunk = createChunk(
            x / CHUNK_SIZE,
            y / CHUNK_SIZE,
            z / CHUNK_SIZE
        );
    }

    int localX = x % CHUNK_SIZE;
    int localY = y % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;

    chunk->setBlock(localX, localY, localZ, blockId);
}

void World::setBlocks(const std::vector<glm::ivec3>& positions, unsigned char blockId) {
    for (const auto& pos : positions) {
        setBlock(pos.x, pos.y, pos.z, blockId);
    }
}

std::optional<WorldRayHit> World::findRayHitBlock(const Ray& ray, float maxDistance) const {
    PERFORM_DDA(ray.origin, ray.direction, maxDistance, {
        if(getBlock(voxel.x, voxel.y, voxel.z) != 0) {
            WorldRayHit hit;

            hit.block = voxel;
            hit.side = side;
            hit.distance = distance;

            return hit;
        }
    });

    return std::nullopt;
}

std::optional<WorldRayHit> World::findRayHitXPlane(const Ray& ray, float maxDistance, int x_plane) const {
    if (ray.direction.x == 0.0f) {
        return std::nullopt;
    }

    float t = (static_cast<float>(x_plane) - ray.origin.x) / ray.direction.x;
    if (t < 0.0f || t > maxDistance) {
        return std::nullopt;
    }

    glm::vec3 intersectionPoint = ray.origin + ray.direction * t;
    glm::ivec3 voxel(
        x_plane,
        static_cast<int>(std::floor(intersectionPoint.y)),
        static_cast<int>(std::floor(intersectionPoint.z))
    );

    WorldRayHit hit;

    hit.block = voxel + glm::ivec3(1, 0, 0);
    hit.side = glm::ivec3((ray.direction.x > 0) ? -1 : 1, 0, 0);
    hit.distance = t;

    return hit;
}

std::optional<WorldRayHit> World::findRayHitYPlane(const Ray& ray, float maxDistance, int y_plane) const {
    if (ray.direction.y == 0.0f) {
        return std::nullopt;
    }

    float t = (static_cast<float>(y_plane) - ray.origin.y) / ray.direction.y;
    if (t < 0.0f || t > maxDistance) {
        return std::nullopt;
    }

    glm::vec3 intersectionPoint = ray.origin + ray.direction * t;
    glm::ivec3 voxel(
        static_cast<int>(std::floor(intersectionPoint.x)),
        y_plane,
        static_cast<int>(std::floor(intersectionPoint.z))
    );

    WorldRayHit hit;

    hit.block = voxel;
    hit.side = glm::ivec3(0, 0, 0);
    hit.distance = t;
    
    return hit;
}

std::optional<WorldRayHit> World::findRayHitXZPlane(const Ray& ray, float maxDistance, glm::ivec2 xz_plane) const {
    if (ray.direction.x == 0.0f && ray.direction.z == 0.0f) {
        return std::nullopt;
    }

    float tX = (ray.direction.x != 0.0f) ? (static_cast<float>(xz_plane.x) - ray.origin.x) / ray.direction.x : std::numeric_limits<float>::infinity();
    float tZ = (ray.direction.z != 0.0f) ? (static_cast<float>(xz_plane.y) - ray.origin.z) / ray.direction.z : std::numeric_limits<float>::infinity();

    float t = std::min(tX, tZ);
    if (t < 0.0f || t > maxDistance) {
        return std::nullopt;
    }

    glm::vec3 intersectionPoint = ray.origin + ray.direction * t;
    glm::ivec3 voxel(
        xz_plane.x,
        static_cast<int>(std::floor(intersectionPoint.y)),
        xz_plane.y
    );

    WorldRayHit hit;

    hit.block = voxel - glm::ivec3((t == tX && ray.direction.x > 0) ? 1 : 0, 0, (t == tZ && ray.direction.z > 0) ? 1 : 0);
    hit.side = glm::ivec3((t == tX) ? ((ray.direction.x > 0) ? -1 : 1) : 0,
                          0,
                          (t == tZ) ? ((ray.direction.z > 0) ? -1 : 1) : 0);
    hit.distance = t;

    return hit;
}

std::vector<glm::ivec3> World::getVoxelsInLine(const glm::ivec3& start, const glm::ivec3& end) const {
    std::vector<glm::ivec3> voxels;
    forVoxelsInLine(start, end, [&](int x, int y, int z) {
        voxels.emplace_back(x, y, z);
    });
    return voxels;
}

std::vector<glm::ivec3> World::getVoxelsInSphere(const glm::ivec3& center, int radius) const {
    std::vector<glm::ivec3> voxels;

    int minX = center.x - radius;
    int maxX = center.x + radius;
    int minY = center.y - radius;
    int maxY = center.y + radius;
    int minZ = center.z - radius;
    int maxZ = center.z + radius;

    int radiusSquared = radius * radius;

    for (int x = minX; x <= maxX; ++x) {
        if(x < 0) continue;

        for (int y = minY; y <= maxY; ++y) {
            if(y < 0) continue;

            for (int z = minZ; z <= maxZ; ++z) {
                if(z < 0) continue;

                glm::ivec3 current(x, y, z);
                glm::ivec3 diff = current - center;

                if((diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) <= radiusSquared) {
                    voxels.emplace_back(x, y, z);
                }
            }
        }
    }

    return voxels;
}

std::vector<glm::ivec3> World::getVoxelsInCube(const glm::ivec3& min, const glm::ivec3& max) const {
    std::vector<glm::ivec3> voxels;

    for (int x = min.x; x <= max.x; ++x) {
        if(x < 0) continue;

        for (int y = min.y; y <= max.y; ++y) {
            if(y < 0) continue;

            for (int z = min.z; z <= max.z; ++z) {
                if(z < 0) continue;

                voxels.emplace_back(x, y, z);
            }
        }
    }

    return voxels;
}

void World::forVoxelsInLine(const glm::ivec3& start, const glm::ivec3& end, const std::function<void(int x, int y, int z)>& action) const {
    glm::vec3 origin = glm::vec3(start) + glm::vec3(0.5f);
    glm::vec3 endPos = glm::vec3(end) + glm::vec3(0.5f);
    glm::vec3 direction = endPos - origin;

    float length = glm::length(direction);
    if (length == 0.0f) {
        action(origin.x, origin.y, origin.z);
        return;
    }

    direction = glm::normalize(direction);
    glm::ivec3 voxel(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );

    float distance = length;
    while(true) {
        action(voxel.x, voxel.y, voxel.z);
        if (distance <= 0.0f) {
            break;
        }

        float step = distance < 1.0f ? distance : 1.0f;
        origin += direction * step;
        voxel = glm::ivec3(
            static_cast<int>(std::floor(origin.x)),
            static_cast<int>(std::floor(origin.y)),
            static_cast<int>(std::floor(origin.z))
        );

        distance -= step;
    }
}

std::vector<glm::ivec3> World::getConnectedVoxels(const glm::ivec3& start) const {
    std::vector<glm::ivec3> connectedVoxels;
    std::unordered_map<uint64_t, bool> visited;

    std::vector<glm::ivec3> stack;
    stack.push_back(start);

    while(!stack.empty()) {
        glm::ivec3 current = stack.back();
        stack.pop_back();

        if(current.x < 0 || current.y < 0 || current.z < 0) {
            continue;
        }

        uint64_t key = getChunkKey(current.x, current.y, current.z);
        if (visited.find(key) != visited.end()) {
            continue;
        }

        visited[key] = true;
        if (getBlock(current.x, current.y, current.z) == 0) {
            continue;
        }

        connectedVoxels.push_back(current);

        stack.push_back(current + glm::ivec3(1, 0, 0));
        stack.push_back(current + glm::ivec3(-1, 0, 0));
        stack.push_back(current + glm::ivec3(0, 1, 0));
        stack.push_back(current + glm::ivec3(0, -1, 0));
        stack.push_back(current + glm::ivec3(0, 0, 1));
        stack.push_back(current + glm::ivec3(0, 0, -1));
        stack.push_back(current + glm::ivec3(1, 1, 0));
        stack.push_back(current + glm::ivec3(-1, -1, 0));
        stack.push_back(current + glm::ivec3(1, 0, 1));
        stack.push_back(current + glm::ivec3(-1, 0, -1));
        stack.push_back(current + glm::ivec3(0, 1, 1));
        stack.push_back(current + glm::ivec3(0, -1, -1));
    }

    return connectedVoxels;
}

WorldAsset World::saveToAsset(const World& world) {
    WorldAsset asset;

    for (const auto& [key, chunk] : world._chunks) {
        ChunkData chunkData;
        chunkData.x = chunk->getX();
        chunkData.y = chunk->getY();
        chunkData.z = chunk->getZ();

        auto blocks = chunk->getBlocks();
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                for (int z = 0; z < CHUNK_SIZE; ++z) {
                    chunkData.data[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] = blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
                }
            }
        }

        asset.chunks[key] = chunkData;
    }

    return asset;
}

std::unique_ptr<World> World::loadFromAsset(const WorldAsset& asset) {
    auto world = std::make_unique<World>();

    for (const auto& [key, chunkData] : asset.chunks) {
        Chunk* chunk = world->createChunk(chunkData.x, chunkData.y, chunkData.z);

        for (int x = 0; x < CHUNK_SIZE; ++x) {
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                for (int z = 0; z < CHUNK_SIZE; ++z) {
                    unsigned char blockType = chunkData.data[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
                    chunk->setBlock(x, y, z, blockType);
                }
            }
        }
    }

    return world;
}