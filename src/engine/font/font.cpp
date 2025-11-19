#include "engine/font/font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

/*using namespace font;

FontFace _createFontFace(FT_Face face, int pixelSize) {
    const int TEXTURE_SIZE = 1024;

    FT_Error error = FT_Set_Pixel_Sizes(face, 0, pixelSize);
    if(error) {
        throw std::runtime_error("Failed to set pixel size for font face.");
    }

    unsigned char* atlasData = new unsigned char[TEXTURE_SIZE * TEXTURE_SIZE];
    memset(atlasData, 0, TEXTURE_SIZE * TEXTURE_SIZE);

    FontFace fontFace;
    auto& glyphs = fontFace.glyphs;

    const int padding = 0;
    int x = padding, y = padding, row_height = 0;

    for(unsigned char c = 0; c < 128; ++c) {
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if(error) {
            throw std::runtime_error("Failed to load character glyph.");
        }

        FT_GlyphSlot glyph = face->glyph;
        const auto buffer = glyph->bitmap.buffer;

        if(x + glyph->bitmap.width + padding > TEXTURE_SIZE) {
            x = padding;
            y += row_height + padding;
            row_height = 0;
        } else if(glyph->bitmap.rows > row_height) {
            row_height = glyph->bitmap.rows;
        }

        if(y + glyph->bitmap.rows + padding > TEXTURE_SIZE) {
            throw std::runtime_error("Font face texture size exceeded.");
        }

        FontGlyph character;

        character.width = glyph->bitmap.width;
        character.height = glyph->bitmap.rows;
        character.bearing_x = glyph->bitmap_left;
        character.bearing_y = glyph->bitmap_top;
        character.offset_x = x;
        character.offset_y = y;
        character.advance_x = glyph->advance.x >> 6;
        character.advance_y = glyph->advance.y >> 6;

        for(int row = 0; row < glyph->bitmap.rows; ++row) {
            for(int col = 0; col < glyph->bitmap.width; ++col) {
                int atlasIndex = (y + row) * TEXTURE_SIZE + (x + col);
                atlasData[atlasIndex] = buffer[row * glyph->bitmap.width + col];
            }
        }

        glyphs.emplace(c, character);

        x += glyph->bitmap.width + padding;
    }    

    fontFace.atlas = std::make_shared<gfx::Texture>(atlasData, TEXTURE_SIZE, TEXTURE_SIZE, 1);
    fontFace.width = TEXTURE_SIZE;
    fontFace.height = TEXTURE_SIZE;

    return fontFace;
}

struct Font::Impl {
    ~Impl() {
        if(face) {
            FT_Done_Face(face);
        }
    }

    FT_Library getLibrary() {
        static FT_Library library = nullptr;
        if (!library) {
            FT_Init_FreeType(&library);
        }

        return library;
    }

    FT_Face face;
};

Font::Font(const std::shared_ptr<assets::Font>& fontAsset)
    : _fontAsset(fontAsset) {
    _impl = new Impl();
    auto& rawData = _fontAsset->getRawData();

    FT_Error error = FT_New_Memory_Face(
        _impl->getLibrary(),
        reinterpret_cast<const FT_Byte*>(rawData.data()),
        static_cast<FT_Long>(rawData.size()),
        0,
        &_impl->face
    );

    if(error) {
        throw std::runtime_error("Failed to create font face from asset.");
    }
}

Font::~Font() {
    if(_impl) {
        delete _impl;
    }
}*/