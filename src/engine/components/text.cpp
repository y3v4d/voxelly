#include "engine/components/text.hpp"

int Text::getContentWidth() const {
    if(!_font) {
        return 0;
    }

    const auto& fontPage = _font->getPage(_fontSize);
    int width = 0;

    for(char c : _content) {
        auto glyphIt = fontPage.glyphs.find(c);
        if(glyphIt != fontPage.glyphs.end()) {
            width += glyphIt->second.advance_x;
        }
    }

    return width;
}