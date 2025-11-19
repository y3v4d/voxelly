#include "engine/systems/ui_renderer.hpp"

#include <iostream>

std::unique_ptr<gfx::VertexArray> createQuadVAO(gfx::BufferUsage usage) {
    const float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
    };

    const int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    auto indexBuffer = std::make_unique<gfx::IndexBuffer>();
    auto vertexBuffer = std::make_unique<gfx::VertexBuffer>(gfx::BufferLayout({
        gfx::BufferLayoutElement(gfx::BufferDataType::FLOAT2),
        gfx::BufferLayoutElement(gfx::BufferDataType::FLOAT2)
    }), usage);

    vertexBuffer->setData(vertices, sizeof(vertices) * sizeof(float));
    indexBuffer->setData(indices, sizeof(indices) * sizeof(int));

    auto vao = std::make_unique<gfx::VertexArray>();
    vao->setVertexBuffer(std::move(vertexBuffer));
    vao->setIndexBuffer(std::move(indexBuffer));

    return vao;
}

void updateQuadVAOUVs(gfx::VertexArray& vao, const glm::vec4& uvs) {
    float vertices[] = {
        0.0f, 0.0f, uvs.x, uvs.y,
        1.0f, 0.0f, uvs.z, uvs.y,
        1.0f, 1.0f, uvs.z, uvs.w,
        0.0f, 1.0f, uvs.x, uvs.w,
    };

    std::cout << "Updating Quad VAO UVs to: " << uvs.x << ", " << uvs.y << ", " << uvs.z << ", " << uvs.w << std::endl;

    vao.getVertexBuffer()->setData(vertices, sizeof(vertices) * sizeof(float));
}

UIRenderer::UIRenderer(gfx::Renderer& renderer, const Camera& camera)
    : _renderer(renderer), _camera(camera) {
    _quadVAO = createQuadVAO(gfx::BufferUsage::STATIC);
    _spriteVAO = createQuadVAO(gfx::BufferUsage::DYNAMIC);
}

void UIRenderer::renderQuad(gfx::Shader& shader, const gfx::Texture& texture, const Transform2D& transform) {
    auto correctedPosition = transform.position - (transform.anchor * transform.size);
    renderQuad(shader, texture, correctedPosition.x, correctedPosition.y, transform.size.x, transform.size.y, transform.rotation);
}

void UIRenderer::renderQuad(gfx::Shader& shader, const gfx::Texture& texture, float x, float y, float width, float height, float angle) {
    _renderer.useShader(shader);
    _renderer.bindTexture(0, texture);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f)) *
                        glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(width, height, 1.0f));

    shader.setUniformMat4("model", glm::value_ptr(model));
    shader.setUniformMat4("view", glm::value_ptr(_camera.getViewMatrix()));
    shader.setUniformMat4("projection", glm::value_ptr(_camera.getProjectionMatrix()));
    
    _renderer.drawVAO(*_quadVAO, gfx::RenderMode::TRIANGLES);
}

void UIRenderer::renderSprite(gfx::Shader& shader, const Sprite& sprite) {
    auto correctedPosition = sprite.position - (sprite.anchor * sprite.size);
    updateQuadVAOUVs(*_spriteVAO, sprite.getUVs());

    _renderer.useShader(shader);
    _renderer.bindTexture(0, *sprite.getTexture());

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(correctedPosition, 0.0f)) *
                        glm::rotate(glm::mat4(1.0f), glm::radians(sprite.rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(sprite.size, 1.0f));

    shader.setUniformMat4("model", glm::value_ptr(model));
    shader.setUniformMat4("view", glm::value_ptr(_camera.getViewMatrix()));
    shader.setUniformMat4("projection", glm::value_ptr(_camera.getProjectionMatrix()));
    shader.setUniformInt("useColor", 0);

    _renderer.drawVAO(*_spriteVAO, gfx::RenderMode::TRIANGLES);
}