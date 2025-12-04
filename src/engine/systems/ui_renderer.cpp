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

    vertexBuffer->setData(vertices, sizeof(vertices));
    indexBuffer->setData(indices, sizeof(indices));

    auto vao = std::make_unique<gfx::VertexArray>();
    vao->setVertexBuffer(std::move(vertexBuffer));
    vao->setIndexBuffer(std::move(indexBuffer));

    return vao;
}

void updateQuadVAOUVs(gfx::VertexArray& vao, const glm::vec4& uvs) {
    const float vertices[] = {
        0.0f, 0.0f, uvs.x, uvs.y,
        1.0f, 0.0f, uvs.z, uvs.y,
        1.0f, 1.0f, uvs.z, uvs.w,
        0.0f, 1.0f, uvs.x, uvs.w,
    };
    
    vao.getVertexBuffer()->setData(vertices, sizeof(vertices));
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

    shader.setUniformMat4("u_Model", glm::value_ptr(model));
    shader.setUniformMat4("u_View", glm::value_ptr(_camera.getViewMatrix()));
    shader.setUniformMat4("u_Projection", glm::value_ptr(_camera.getProjectionMatrix()));
    
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

    auto mvp = _camera.getProjectionMatrix() * _camera.getViewMatrix() * model;
    auto color = sprite.getColor();

    shader.setUniformMat4("u_MVP", glm::value_ptr(mvp));
    shader.setUniformVec4("u_Color", color.r, color.g, color.b, color.a);
    shader.setUniformInt("u_UseColor", 1);
    shader.setUniformInt("u_UseTexture", 1);

    _renderer.drawVAO(*_spriteVAO, gfx::RenderMode::TRIANGLES);
}