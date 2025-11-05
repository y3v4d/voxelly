#pragma once

#include "engine/engine.hpp"

class TextRenderer {
public:
    TextRenderer(gfx::Renderer& renderer) : _renderer(renderer) {}

    void renderText(Text& text, float x, float y);
private:
    gfx::Renderer& _renderer;
    std::unordered_map<const Text*, unique<gfx::VertexArray>> _textVaos;
    std::unordered_map<const assets::Font::FontPage*, unique<gfx::Texture>> _fontPageTextures;
};