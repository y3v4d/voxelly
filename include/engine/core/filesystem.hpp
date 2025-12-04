#pragma once

#include "engine/core/core.hpp"

namespace core {
    class FileSystem {
    public:
        static std::string readFromFile(const std::string& path);
        static void writeToFile(const std::string& path, const std::string& content);

        static std::string getExecutablePath();
        static std::string getDataPath();

    private:
        FileSystem() = default;
        ~FileSystem() = default;
    };
}