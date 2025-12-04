#include "engine/core/filesystem.hpp"

#include <iostream>
#include <fstream>

#include <SDL3/SDL.h>

using namespace core;

std::string FileSystem::readFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');

    if(!file.read(&buffer[0], size)) {
        throw std::runtime_error("Failed to read file: " + path);
    }

    return buffer;
}

void FileSystem::writeToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary | std::ios::out);
    if(!file) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }

    file.write(content.data(), content.size());
    if(!file) {
        throw std::runtime_error("Failed to write to file: " + path);
    }

    file.close();
}

std::string FileSystem::getExecutablePath() {
    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        return std::string(basePath);
    }

    return std::string("./");
}

std::string FileSystem::getDataPath() {
    const char* dataPath = SDL_GetPrefPath("y3v4d", "voxelly");
    if (dataPath) {
        return std::string(dataPath);
    }

    return std::string("./");
}