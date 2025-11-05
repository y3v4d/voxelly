#pragma once

#include <string>

namespace assets {
    class FileAsset {
    public:
        FileAsset(const std::string& path);
        ~FileAsset();
        
        void load();
        
        const char* getData() const { return data; }
        size_t getSize() const { return size; }

        const std::string& getPath() const { return path; }
    private:
        char* data;
        size_t size;

        std::string path;
    };
}