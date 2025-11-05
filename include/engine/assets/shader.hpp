#pragma once

#include "engine/core/core.hpp"

namespace assets {
    class Shader {
    public:
        Shader() = default;
        Shader(const std::string& path);
        Shader(const std::string& vertexSource, const std::string& fragmentSource)
            : _vertexSource(vertexSource), _fragmentSource(fragmentSource) {}

        const std::string& getVertexSource() const {
            return _vertexSource;
        }

        const std::string& getFragmentSource() const {
            return _fragmentSource;
        }
    private:
        std::string _vertexSource;
        std::string _fragmentSource;
    };
}