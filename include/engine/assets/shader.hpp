#pragma once

#include "engine/core/core.hpp"
#include "engine/assets/asset.hpp"

namespace assets {
    class Shader : public Asset {
    public:
        Shader() = default;
        Shader(const std::string& vertexSource, const std::string& fragmentSource)
            : _vertexSource(vertexSource), _fragmentSource(fragmentSource) {}

        virtual void loadFromBuffer(const char* data, size_t size) override;

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