#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/gfx/renderer.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"
#include "engine/gfx/buffer/vertex_array.hpp"
#include "engine/components/camera.hpp"
#include "engine/components/transform2d.hpp"
#include "engine/components/sprite.hpp"

class UIRenderer {
public:
    UIRenderer() = delete;
    UIRenderer(gfx::Renderer& renderer, const Camera& camera);

    void renderQuad(gfx::Shader& shader, const gfx::Texture& texture, float x, float y, float width, float height, float angle);
    void renderQuad(gfx::Shader& shader, const gfx::Texture& texture, const Transform2D& transform);

    void renderSprite(gfx::Shader& shader, const Sprite& sprite);

private:
    gfx::Renderer& _renderer;
    const Camera& _camera;

    std::unique_ptr<gfx::VertexArray> _quadVAO;
    std::unique_ptr<gfx::VertexArray> _spriteVAO;
};