#pragma once

#include <memory>
#include <map>

#include "engine/assets/asset.hpp"
#include "engine/core/core.hpp"

namespace assets {
    class Font : public Asset {
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

        ~Font();

        virtual void loadFromBuffer(const char* data, size_t size) override;

        const std::string& getRawData() const { return _rawData; }
        const FontPage& getPage(int pixelSize);

    private:
        struct Impl;
        Impl* _impl;

        std::string _rawData;
        std::unordered_map<int, FontPage> _pages;
    };
}