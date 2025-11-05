#include "engine/assets/shader.hpp"
#include "engine/core/filesystem.hpp"

using namespace assets;

Shader::Shader(const std::string& path) {
    _vertexSource = core::FileSystem::readFromFile(path + ".vert");
    _fragmentSource = core::FileSystem::readFromFile(path + ".frag");
}