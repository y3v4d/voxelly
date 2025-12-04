#include "engine/assets/shader.hpp"
#include "engine/core/filesystem.hpp"

using namespace assets;

// trim function
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void Shader::loadFromBuffer(const char* data, size_t size) {
    std::string buffer(data, size);
    
    size_t vertexPos = buffer.find("#vertex");
    size_t fragmentPos = buffer.find("#fragment");

    if(vertexPos == std::string::npos || fragmentPos == std::string::npos) {
        throw std::runtime_error("Shader source missing #vertex or #fragment markers");
    }

    _vertexSource = trim(buffer.substr(vertexPos + 7, fragmentPos - (vertexPos + 7)));
    _fragmentSource = trim(buffer.substr(fragmentPos + 9));
}