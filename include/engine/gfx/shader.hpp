#pragma once

#include "engine/assets/shader.hpp"

namespace gfx {
    enum ShaderType {
        VERTEX,
        FRAGMENT
    };

    class Shader {
    public:
        Shader();
        ~Shader();

        void compile(const assets::Shader& shader);
        void compile(const char* vertex_source, const char* fragment_source);
        
        void setUniformMat4(const char* name, const float* matrix);
        void setUniformVec3(const char* name, float x, float y, float z);
        void setUniformVec4(const char* name, float x, float y, float z, float w);
        void setUniformFloat(const char* name, float value);
        void setUniformInt(const char* name, int value);

        unsigned int getID() const { return _id; }

    private:
        unsigned int _id;
    };
}