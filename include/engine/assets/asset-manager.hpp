#pragma once

#include <map>
#include <memory>
#include "engine/core/filesystem.hpp"
#include "engine/assets/asset.hpp"

namespace assets {
    class AssetManager {
    public:
        template<typename T>
        std::shared_ptr<T> loadAsset(const std::string& path, bool forceReload = false) {
            static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset");

            auto it = _assets.find(path);
            if(it != _assets.end()) {
                if(forceReload) {
                    auto buffer = core::FileSystem::readFromFile(path);
                    it->second->loadFromBuffer(buffer.data(), buffer.size());
                }

                return std::dynamic_pointer_cast<T>(it->second);
            }

            std::shared_ptr<T> asset = std::make_shared<T>();
            auto buffer = core::FileSystem::readFromFile(path);
            
            asset->loadFromBuffer(buffer.data(), buffer.size());
            _assets.emplace(path, asset);

            return asset;
        }

        void releaseAsset(const std::string& path) {
            _assets.erase(path);
        }

        void releaseAllAssets() {
            _assets.clear();
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Asset>> _assets;
    };
}