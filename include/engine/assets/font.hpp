#pragma once

#include <memory>
#include <map>

#include "engine/core/core.hpp"

namespace assets {
    class Font {
    public:
        struct Glyph {
            int width;
            int height;

            int bearing_x;
            int bearing_y;

            int advance_x;
            int advance_y;

            int offset_x;
            int offset_y;
        };

        struct FontPage {
            ~FontPage();

            unsigned char* atlas = nullptr;

            int width = 0;
            int height = 0;

            std::unordered_map<char, Glyph> glyphs;
        };

        Font() = delete;

        Font(const std::string& path);
        ~Font();

        const FontPage& getPage(int pixelSize);

        Font& operator=(const Font&) = delete;

    private:
        struct Impl;
        Impl* _impl;

        std::string _rawData;
        std::unordered_map<int, FontPage> _pages;
    };
}