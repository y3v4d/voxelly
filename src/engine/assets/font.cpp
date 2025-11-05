#include "engine/assets/font.hpp"
#include "engine/core/filesystem.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace assets;

Font::FontPage::~FontPage() {
}

Font::FontPage _createFontPage(FT_Face face, int pixelSize) {
    const int TEXTURE_SIZE = 1024;

    FT_Error error = FT_Set_Pixel_Sizes(face, 0, pixelSize);
    if(error) {
        throw std::runtime_error("Failed to set pixel size for font face. FreeType error code: " + std::to_string(error));
    }

    Font::FontPage page;

    page.atlas = new unsigned char[TEXTURE_SIZE * TEXTURE_SIZE];
    page.width = TEXTURE_SIZE;
    page.height = TEXTURE_SIZE;

    auto atlas = page.atlas;
    auto& glyphs = page.glyphs;
    memset(atlas, 0, TEXTURE_SIZE * TEXTURE_SIZE);

    const int padding = 0;
    int x = padding, y = padding, row_height = 0;

    for(unsigned char c = 0; c < 128; ++c) {
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if(error) {
            throw std::runtime_error("Failed to load character " + std::to_string(c) + " glyph. FreeType error code: " + std::to_string(error));
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
            throw std::runtime_error("Font page size exceeded.");
        }

        Font::Glyph character;

        character.width = glyph->bitmap.width;
        character.height = glyph->bitmap.rows;
        character.bearing_x = glyph->bitmap_left;
        character.bearing_y = glyph->bitmap_top;
        character.offset_x = x;
        character.offset_y = y;
        character.advance_x = glyph->advance.x >> 6;
        character.advance_y = glyph->advance.y >> 6;

        glyphs.emplace(c, character);

        for(int row = 0; row < character.height; ++row) {
            for(int col = 0; col < character.width; ++col) {
                atlas[(TEXTURE_SIZE - 1 - (y + row)) * TEXTURE_SIZE + (x + col)] = buffer[row * character.width + col];
            }
        }

        x += character.width + padding;
    }

    return page;
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

Font::Font(const std::string& path) {
    _rawData = core::FileSystem::readFromFile(path);
    _impl = new Impl();

    FT_Error error = FT_New_Memory_Face(
        _impl->getLibrary(),
        (const FT_Byte*)_rawData.data(),
        (FT_Long)_rawData.size(),
        0,
        &_impl->face
    );

    if(error) {
        throw std::runtime_error("Failed to create font face from memory buffer. FreeType error code: " + std::to_string(error));
    }

    error = FT_Select_Charmap(_impl->face, FT_ENCODING_UNICODE);
    if(error) {
        throw std::runtime_error("Failed to select Unicode charmap for font face. FreeType error code: " + std::to_string(error));
    }
}

Font::~Font() {
    if(_impl) {
        delete _impl;
    }
}

const Font::FontPage& Font::getPage(int pixelSize) {
    auto it = _pages.find(pixelSize);
    if(it != _pages.end()) {
        return it->second;
    }

    auto page = _createFontPage(_impl->face, pixelSize);
    _pages.emplace(pixelSize, page);

    return _pages[pixelSize];
}

