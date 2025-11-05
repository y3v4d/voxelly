#include "engine/gfx/renderer.hpp"

using namespace gfx;

unique<VertexArray> Renderer::createMeshVAO(const assets::Mesh& mesh) {
    std::vector<BufferLayoutElement> elements;

    elements.push_back(BufferLayoutElement(BufferDataType::FLOAT3));
    elements.push_back(BufferLayoutElement(BufferDataType::FLOAT2));
    elements.push_back(BufferLayoutElement(BufferDataType::FLOAT3));

    BufferLayout layout(elements);
    size_t vertexCount = mesh.positions.size() / 3;

    auto vertexBuffer = std::make_unique<VertexBuffer>(layout);
    auto vertexData = std::make_unique<float[]>(vertexCount * (3 + 2 + 3));

    size_t offset = 0;
    for (size_t i = 0; i < vertexCount; ++i) {
        if(!mesh.positions.empty()) {
            vertexData[offset++] = mesh.positions[i * 3];
            vertexData[offset++] = mesh.positions[i * 3 + 1];
            vertexData[offset++] = mesh.positions[i * 3 + 2];
        } else {
            vertexData[offset++] = 0.0f;
            vertexData[offset++] = 0.0f;
            vertexData[offset++] = 0.0f;
        }

        if(!mesh.texCoords.empty()) {
            vertexData[offset++] = mesh.texCoords[i * 2];
            vertexData[offset++] = mesh.texCoords[i * 2 + 1];
        } else {
            vertexData[offset++] = 0.0f;
            vertexData[offset++] = 0.0f;
        }

        if(!mesh.normals.empty()) {
            vertexData[offset++] = mesh.normals[i * 3];
            vertexData[offset++] = mesh.normals[i * 3 + 1];
            vertexData[offset++] = mesh.normals[i * 3 + 2];
        } else {
            vertexData[offset++] = 0.0f;
            vertexData[offset++] = 0.0f;
            vertexData[offset++] = 0.0f;
        }
    }

    vertexBuffer->setData(vertexData.get(), offset * sizeof(float));

    auto indexBuffer = std::make_unique<IndexBuffer>();
    indexBuffer->setData(mesh.indices.data(), static_cast<uint32_t>(mesh.indices.size()) * sizeof(unsigned int));

    auto vao = std::make_unique<VertexArray>();
    vao->setVertexBuffer(std::move(vertexBuffer));
    vao->setIndexBuffer(std::move(indexBuffer));

    return vao;
}