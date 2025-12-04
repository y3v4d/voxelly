#pragma once

#include "chunk.hpp"
#include "world_asset.hpp"
#include "ray.hpp"

struct WorldRayHit {
    glm::ivec3 block;
    glm::ivec3 side;
    
    float distance;
};

class World {
public:
    World() = default;
    ~World() = default;

    Chunk* createChunk(int x, int y, int z);
    Chunk* getChunk(int x, int y, int z) const;

    Chunk* getChunkContainingBlock(int x, int y, int z) const;
    const auto& getChunks() const { return _chunks; }
    void removeChunk(int x, int y, int z);
    void removeAllChunks();

    unsigned char getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, unsigned char blockId);
    void setBlocks(const std::vector<glm::ivec3>& positions, unsigned char blockId);

    std::optional<WorldRayHit> findRayHitBlock(const Ray& ray, float maxDistance) const;
    std::optional<WorldRayHit> findRayHitXPlane(const Ray& ray, float maxDistance, int x_plane) const;
    std::optional<WorldRayHit> findRayHitYPlane(const Ray& ray, float maxDistance, int y_plane) const;
    std::optional<WorldRayHit> findRayHitXZPlane(const Ray& ray, float maxDistance, glm::ivec2 xz_plane) const;
    //std::optional<WorldRayHit> findRayHitOnAxis(const Ray& ray, float maxDistance, int axis, int axis_value) const;
    std::vector<glm::ivec3> getVoxelsInLine(const glm::ivec3& start, const glm::ivec3& end) const;
    std::vector<glm::ivec3> getVoxelsInSphere(const glm::ivec3& center, int radius) const;
    std::vector<glm::ivec3> getVoxelsInCube(const glm::ivec3& min, const glm::ivec3& max) const;
    std::vector<glm::ivec3> getConnectedVoxels(const glm::ivec3& start) const;
    void forVoxelsInLine(const glm::ivec3& start, const glm::ivec3& end, const std::function<void(int x, int y, int z)>& action) const;

    static std::unique_ptr<World> loadFromAsset(const WorldAsset& asset);
    static WorldAsset saveToAsset(const World& world);

private:
    std::unordered_map<uint64_t, std::unique_ptr<Chunk>> _chunks;
};

uint64_t getChunkKey(int x, int y, int z);