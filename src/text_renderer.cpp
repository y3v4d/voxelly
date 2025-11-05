#include "text_renderer.hpp"

#include "engine/core/core.hpp"

void updateTextVao(gfx::VertexArray& vao, const Text& text) {
    auto& content = text.getContent();
    auto font = text.getFont();
    auto fontSize = text.getFontSize();

    const int numVertices = content.size() * 4;
    int numIndices = content.size() * 6;

    auto vertexBuffer = vao.getVertexBuffer().get();
    auto indexBuffer = vao.getIndexBuffer().get();

    float* vertices = new float[numVertices * 4]; // 4 floats per vertex (x, y, u, v)
    unsigned int* indices = new unsigned int[numIndices];

    auto& fontPage = font->getPage(fontSize);

    float x = 0.0f;
    float y = 0.0f;
    int vertexOffset = 0;
    int indexOffset = 0;

    for(int i = 0; i < content.size(); ++i) {
        char c = content[i];
        auto& character = fontPage.glyphs.at(c);

        float xpos = x + character.bearing_x;
        float ypos = y - (character.height - character.bearing_y);

        float w = character.width;
        float h = character.height;

        float uv_x = static_cast<float>(character.offset_x) / fontPage.width;
        float uv_y = static_cast<float>(fontPage.height - character.offset_y - character.height) / fontPage.height;
        float uv_w = static_cast<float>(character.width) / fontPage.width;
        float uv_h = static_cast<float>(character.height) / fontPage.height;

        vertices[vertexOffset + 0] = xpos;
        vertices[vertexOffset + 1] = ypos;
        vertices[vertexOffset + 2] = uv_x;
        vertices[vertexOffset + 3] = uv_y;

        vertices[vertexOffset + 4] = xpos + w;
        vertices[vertexOffset + 5] = ypos;
        vertices[vertexOffset + 6] = uv_x + uv_w;
        vertices[vertexOffset + 7] = uv_y;

        vertices[vertexOffset + 8] = xpos + w;
        vertices[vertexOffset + 9] = ypos + h;
        vertices[vertexOffset + 10] = uv_x + uv_w;
        vertices[vertexOffset + 11] = uv_y + uv_h;

        vertices[vertexOffset + 12] = xpos;
        vertices[vertexOffset + 13] = ypos + h;
        vertices[vertexOffset + 14] = uv_x;
        vertices[vertexOffset + 15] = uv_y + uv_h;

        indices[indexOffset + 0] = i * 4 + 0;
        indices[indexOffset + 1] = i * 4 + 1;
        indices[indexOffset + 2] = i * 4 + 2;
        indices[indexOffset + 3] = i * 4 + 2;
        indices[indexOffset + 4] = i * 4 + 3;
        indices[indexOffset + 5] = i * 4 + 0;

        x += character.advance_x;
        y += character.advance_y;

        vertexOffset += 16;
        indexOffset += 6;
    }

    vertexBuffer->setData(vertices, numVertices * 4 * sizeof(float));
    indexBuffer->setData(indices, numIndices * sizeof(unsigned int));

    delete[] vertices;
    delete[] indices;
}

unique<gfx::VertexArray> createTextVAO(const Text& text) {
    auto vao = std::make_unique<gfx::VertexArray>();
    
    auto vertexBuffer = std::make_unique<gfx::VertexBuffer>(gfx::BufferLayout({
        { gfx::BufferDataType::FLOAT2 }, // position
        { gfx::BufferDataType::FLOAT2 }  // texcoord
    }));
    auto indexBuffer = std::make_unique<gfx::IndexBuffer>();

    vao->setVertexBuffer(std::move(vertexBuffer));
    vao->setIndexBuffer(std::move(indexBuffer));

    updateTextVao(*vao, text);

    return vao;
}

void TextRenderer::renderText(Text& text, float x, float y) {
    if(_textVaos.find(&text) == _textVaos.end()) {
        _textVaos[&text] = createTextVAO(text);
    }

    auto page = text.getFont()->getPage(text.getFontSize());
    auto it = _fontPageTextures.find(&page);
    if(it == _fontPageTextures.end()) {
        auto texture = std::make_unique<gfx::Texture>((char*)page.atlas, page.width, page.height, 1);
        if(!texture) {
            throw std::runtime_error("Failed to create font page texture.");
        }

        _fontPageTextures[&page] = std::move(texture);
    }

    if(text._dirty) {
        updateTextVao(*_textVaos[&text], text);
        text._dirty = false;
    }

    _renderer.bindTexture(0, *_fontPageTextures[&page]);
    _renderer.drawVAO(*_textVaos[&text], gfx::RenderMode::TRIANGLES);
}