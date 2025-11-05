#pragma once

#include <string>

#include "engine/core/core.hpp"
#include "engine/assets/font.hpp"
#include "engine/components/transform.hpp"

class TextRenderer;
class Text : public Transform {
public:
    Text() : _content(""), _font(nullptr), _fontSize(0) {}
    Text(const std::string& content, shared<assets::Font> font, int fontSize)
        : _content(content), _font(font), _fontSize(fontSize) {}

    void setContent(const std::string& content) {
        _content = content;
        _dirty = true;
    }

    const std::string& getContent() const {
        return _content;
    }

    shared<assets::Font> getFont() const {
        return _font;
    }

    int getFontSize() const {
        return _fontSize;
    }

private:
    std::string _content;

    shared<assets::Font> _font;
    int _fontSize;

    bool _dirty = true;

    friend class TextRenderer;
};