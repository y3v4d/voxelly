#pragma once

#include "engine/core/core.hpp"
#include <vector>

namespace assets {
    class Mesh {
    public:
        std::vector<float> positions;
        std::vector<float> texCoords;
        std::vector<float> normals;

        std::vector<unsigned int> indices;
    };

    class MeshBuilder {
    public:
        static Mesh createCube();
        static Mesh createOutlineCube();
        static Mesh createPlane();
    };
}