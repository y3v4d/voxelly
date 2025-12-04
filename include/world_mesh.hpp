#pragma once

#include "engine/engine.hpp"
#include "world.hpp"
#include "chunk_mesh.hpp"

class WorldMesh {
public:
    WorldMesh(World* world) : _world(world) {}
    ~WorldMesh() = default;

    void update() {
        for (auto it = _chunkMeshes.begin(); it != _chunkMeshes.end(); ) {
            if (_world->getChunks().find(it->first) == _world->getChunks().end()) {
                it = _chunkMeshes.erase(it);
            } else {
                ++it;
            }
        }

        for (auto& [key, chunk] : _world->getChunks()) {
            if (chunk->isDirty()) {
                if (_chunkMeshes.find(key) == _chunkMeshes.end()) {
                    _chunkMeshes[key] = std::make_unique<ChunkMesh>(chunk.get());
                }

                _chunkMeshes[key]->updateMesh();
                chunk->setDirty(false);
            }
        }
    }

    World* getWorld() const { return _world; }

    std::unordered_map<uint64_t, std::unique_ptr<ChunkMesh>>& getChunkMeshes() {
        return _chunkMeshes;
    }

    void clearChunkMeshes() {
        _chunkMeshes.clear();
    }

private:
    World* _world;
    std::unordered_map<uint64_t, std::unique_ptr<ChunkMesh>> _chunkMeshes;
};