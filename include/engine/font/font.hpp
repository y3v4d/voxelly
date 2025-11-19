#pragma once

#include <map>

#include "engine/gfx/texture.hpp"
#include "engine/assets/font.hpp"

namespace font {
    struct FontGlyph {
        unsigned char* bitmap = nullptr;

        int width;
        int height;

        int bearing_x;
        int bearing_y;

        int advance_x;
        int advance_y;

        int offset_x;
        int offset_y;
    };

    struct FontFace {
        std::unordered_map<char, FontGlyph> glyphs;
    };

    class Font {
    public:
        Font(const std::shared_ptr<assets::Font>& fontAsset);
        ~Font();

        const FontFace& getFace(int pixelSize);

    private:
        struct Impl;
        Impl* _impl;

        std::unordered_map<int, FontFace> _faces;
        std::shared_ptr<assets::Font> _fontAsset;
    };
}