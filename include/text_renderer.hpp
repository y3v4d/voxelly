#pragma once

#include "engine/engine.hpp"

class TextRenderer {
public:
    TextRenderer(gfx::Renderer& renderer, const Camera& camera) : _renderer(renderer), _camera(camera) {}

    void renderText(gfx::Shader& shader, Text& text);
private:
    gfx::Renderer& _renderer;
    const Camera& _camera;

    std::unordered_map<const Text*, unique<gfx::VertexArray>> _textVaos;
    std::unordered_map<const assets::Font::FontPage*, unique<gfx::Texture>> _fontPageTextures;
};