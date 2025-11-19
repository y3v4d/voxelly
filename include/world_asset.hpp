#pragma once

#include "chunk.hpp"
#include <map>
#include <string>

struct ChunkData {
    int x;
    int y;
    int z;

    std::array<unsigned char, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> data;
};

class WorldAsset {
public:
    std::string name;
    std::unordered_map<uint64_t, ChunkData> chunks;

    void saveToFile(const std::string& filepath) const;
    static WorldAsset loadFromFile(const std::string& filepath);
};