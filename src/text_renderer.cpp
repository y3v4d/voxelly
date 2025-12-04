#include "text_renderer.hpp"

#include "engine/core/core.hpp"

void computeTextDimensions(const Text& text, float& outWidth, float& outHeight) {
    auto& content = text.getContent();
    auto font = text.getFont();
    auto fontSize = text.getFontSize();

    float width = 0.0f;
    float height = 0.0f;

    auto& fontPage = font->getPage(fontSize);

    for(int i = 0; i < content.size(); ++i) {
        char c = content[i];
        auto& character = fontPage.glyphs.at(c);

        width += (float)character.advance_x;
        height = std::max(height, (float)character.height);
    }

    outWidth = width;
    outHeight = height;
}

void updateTextVao(gfx::VertexArray& vao, Text& text) {
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

        x += (float)character.advance_x;
        y += (float)character.advance_y;

        vertexOffset += 16;
        indexOffset += 6;
    }

    float width, height;
    computeTextDimensions(text, width, height);

    text.size = glm::vec2(width, height);

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

    return vao;
}

void TextRenderer::renderText(gfx::Shader& shader, Text& text) {
    if(_textVaos.find(&text) == _textVaos.end()) {
        _textVaos[&text] = createTextVAO(text);
        updateTextVao(*_textVaos[&text], text);

        text._dirty = false;
    }

    auto page = text.getFont()->getPage(text.getFontSize());
    auto it = _fontPageTextures.find(&page);
    if(it == _fontPageTextures.end()) {
        auto texture = std::make_unique<gfx::Texture>(page.atlas, page.width, page.height, 1);
        if(!texture) {
            throw std::runtime_error("Failed to create font page texture.");
        }

        _fontPageTextures[&page] = std::move(texture);
    }

    if(text._dirty) {
        updateTextVao(*_textVaos[&text], text);
        text._dirty = false;
    }

    auto correctedPosition = text.position - (text.anchor * text.size);
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(correctedPosition, 0.0f)) *
                 glm::rotate(glm::mat4(1.0f), glm::radians(text.rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
                 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    _renderer.useShader(shader);
    _renderer.bindTexture(0, *_fontPageTextures[&page]);

    shader.setUniformMat4("u_Projection", glm::value_ptr(_camera.getProjectionMatrix()));
    shader.setUniformMat4("u_View", glm::value_ptr(_camera.getViewMatrix()));
    shader.setUniformMat4("u_Model", glm::value_ptr(model));

    _renderer.drawVAO(*_textVaos[&text], gfx::RenderMode::TRIANGLES);
}