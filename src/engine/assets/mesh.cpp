#include "engine/assets/mesh.hpp"

#include <iostream>

using namespace assets;

const std::array<float, 8 * 3> CUBE_POSITIONS = {
    // Front face
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f,  0.5f, 0.5f,
    -0.5f,  0.5f, 0.5f,
    
    // Back face
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f
};

const std::array<unsigned int, 6 * 4> CUBE_FACES = {
    0, 1, 2, 3, // Front face
    4, 5, 6, 7, // Back face
    5, 0, 3, 6, // Left face
    1, 4, 7, 2, // Right face
    3, 2, 7, 6, // Top face
    5, 4, 1, 0  // Bottom face
};

const std::array<float, 6 * 3> CUBE_NORMALS = {
    0.0f, 0.0f, 1.0f, // Front face
    0.0f, 0.0f, -1.0f, // Back face
    -1.0f, 0.0f, 0.0f, // Left face
    1.0f, 0.0f, 0.0f,  // Right face
    0.0f, 1.0f, 0.0f,  // Top face
    0.0f, -1.0f, 0.0f // Bottom face
};

const std::array<float, 4 * 2> CUBE_UVS = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

Mesh MeshBuilder::createCube() {
    Mesh mesh;

    std::array<float, 6 * 4 * 3> positions;
    std::array<float, 6 * 4 * 3> normals;
    std::array<float, 6 * 4 * 2> uvs;
    std::array<unsigned int, 6 * 6> indices;

    for (int face = 0; face < 6; ++face) {
        for (int vertex = 0; vertex < 4; ++vertex) {
            int posIndex = face * 4 * 3 + vertex * 3;
            int normIndex = face * 4 * 3 + vertex * 3;
            int uvIndex = face * 4 * 2 + vertex * 2;
            
            // Positions
            positions[posIndex]     = CUBE_POSITIONS[CUBE_FACES[face * 4 + vertex] * 3 + 0];
            positions[posIndex + 1] = CUBE_POSITIONS[CUBE_FACES[face * 4 + vertex] * 3 + 1];
            positions[posIndex + 2] = CUBE_POSITIONS[CUBE_FACES[face * 4 + vertex] * 3 + 2];

            // Normals
            normals[normIndex]     = CUBE_NORMALS[face * 3 + 0];
            normals[normIndex + 1] = CUBE_NORMALS[face * 3 + 1];
            normals[normIndex + 2] = CUBE_NORMALS[face * 3 + 2];

            // UVs
            uvs[uvIndex]     = CUBE_UVS[vertex * 2 + 0];
            uvs[uvIndex + 1] = CUBE_UVS[vertex * 2 + 1];
        }

        // Indices
        int indexOffset = face * 6;
        int vertexOffset = face * 4;

        indices[indexOffset] = vertexOffset;
        indices[indexOffset + 1] = vertexOffset + 1;
        indices[indexOffset + 2] = vertexOffset + 2;
        indices[indexOffset + 3] = vertexOffset + 2;
        indices[indexOffset + 4] = vertexOffset + 3;
        indices[indexOffset + 5] = vertexOffset + 0;
    }

    mesh.positions = std::vector<float>(positions.begin(), positions.end());
    mesh.normals = std::vector<float>(normals.begin(), normals.end());
    mesh.texCoords = std::vector<float>(uvs.begin(), uvs.end());
    mesh.indices = std::vector<unsigned int>(indices.begin(), indices.end());

    return mesh;
}

Mesh MeshBuilder::createOutlineCube() {
    Mesh mesh;

    std::array<float, 8 * 3> position = CUBE_POSITIONS;
    std::array<unsigned int, 12 * 2> indices = {
        0, 1, 1, 2, 2, 3, 3, 0, // Front face
        4, 5, 5, 6, 6, 7, 7, 4, // Back face
        0, 5, 1, 4, 2, 7, 3, 6  // Side edges
    };

    mesh.positions = std::vector<float>(position.begin(), position.end());
    mesh.indices = std::vector<unsigned int>(indices.begin(), indices.end());

    return mesh;
}

Mesh MeshBuilder::createPlane() {
    Mesh mesh;

    std::array<float, 4 * 3> positions = {
        -0.5f, 0.0f, 0.5f,
        0.5f, 0.0f, 0.5f,
        0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f, -0.5f
    };

    std::array<float, 4 * 2> uvs = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    std::array<float, 4 * 3> normals = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    std::array<unsigned int, 6> indices = {
        0, 1, 2,
        2, 3, 0
    };

    mesh.positions = std::vector<float>(positions.begin(), positions.end());
    mesh.texCoords = std::vector<float>(uvs.begin(), uvs.end());
    mesh.normals = std::vector<float>(normals.begin(), normals.end());
    mesh.indices = std::vector<unsigned int>(indices.begin(), indices.end());

    return mesh;
}