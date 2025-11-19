#include "world_asset.hpp"
#include "engine/core/filesystem.hpp"

#include <sstream>

void WorldAsset::saveToFile(const std::string& filepath) const {
    std::stringstream buffer;

    uint32_t nameLength = static_cast<uint32_t>(name.size());
    buffer.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    buffer.write(name.c_str(), nameLength);

    uint32_t chunkCount = static_cast<uint32_t>(chunks.size());
    buffer.write(reinterpret_cast<const char*>(&chunkCount), sizeof(chunkCount));

    for (const auto& [key, chunk] : chunks) {
        buffer.write(reinterpret_cast<const char*>(&key), sizeof(key));
        buffer.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
    }

    core::FileSystem::writeToFile(filepath, buffer.str());
}

WorldAsset WorldAsset::loadFromFile(const std::string& filepath) {
    std::string buffer = core::FileSystem::readFromFile(filepath);
    std::istringstream file(buffer);

    WorldAsset worldAsset;
    uint32_t nameLength;
    
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    worldAsset.name.resize(nameLength);
    file.read(&worldAsset.name[0], nameLength);

    uint32_t chunkCount;
    file.read(reinterpret_cast<char*>(&chunkCount), sizeof(chunkCount));
    for (uint32_t i = 0; i < chunkCount; ++i) {
        uint64_t key;
        ChunkData chunk;
        file.read(reinterpret_cast<char*>(&key), sizeof(key));
        file.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));
        worldAsset.chunks[key] = chunk;
    }

    return worldAsset;
}