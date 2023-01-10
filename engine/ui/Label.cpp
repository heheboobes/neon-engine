#include "Label.h"
#include <sstream>

namespace engine::ui {

Label::Label()
    : Widget()
{
    setSize({200.f, 20.f});
    sfText_.setCharacterSize(fontSize_);
    sfText_.setFillColor(color_);
}

Label::Label(const std::string& text)
    : Widget()
    , text_(text)
{
    setSize({200.f, 20.f});
    sfText_.setString(text_);
    sfText_.setCharacterSize(fontSize_);
    sfText_.setFillColor(color_);
    rebuildText();
}

void Label::update(float dt) {
    Widget::update(dt);
    if (autoSize_) {
        sf::FloatRect bounds = sfText_.getLocalBounds();
        setSize({bounds.width + getPadding().left + getPadding().right,
                 bounds.height + getPadding().top + getPadding().bottom});
    }
}

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());
    target.draw(sfText_, states);
    Widget::draw(target, states);
}

void Label::setText(const std::string& text) {
    text_ = text;
    rebuildText();
}

const std::string& Label::getText() const {
    return text_;
}

void Label::setFont(const sf::Font& font) {
    sfText_.setFont(font);
    rebuildText();
}

void Label::setFontSize(unsigned int size) {
    fontSize_ = size;
    sfText_.setCharacterSize(fontSize_);
    rebuildText();
}

unsigned int Label::getFontSize() const {
    return fontSize_;
}

void Label::setColor(sf::Color color) {
    color_ = color;
    sfText_.setFillColor(color_);
}

sf::Color Label::getColor() const {
    return color_;
}

void Label::setAlignment(TextAlignment alignment) {
    alignment_ = alignment;
    applyAlignment();
}

TextAlignment Label::getAlignment() const {
    return alignment_;
}

void Label::setWordWrap(bool wrap) {
    wordWrap_ = wrap;
    rebuildText();
}

bool Label::getWordWrap() const {
    return wordWrap_;
}

void Label::setAutoSize(bool autoSize) {
    autoSize_ = autoSize;
}

bool Label::getAutoSize() const {
    return autoSize_;
}

void Label::setLineSpacing(float spacing) {
    lineSpacing_ = spacing;
    sfText_.setLineSpacing(lineSpacing_);
}

float Label::getLineSpacing() const {
    return lineSpacing_;
}

void Label::rebuildText() {
    if (wordWrap_ && getSize().x > 0.f) {
        float maxWidth = getSize().x - getPadding().left - getPadding().right;
        auto lines = wrapText(text_, maxWidth);
        std::string wrapped;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i > 0) wrapped += '\n';
            wrapped += lines[i];
        }
        sfText_.setString(wrapped);
    } else {
        sfText_.setString(text_);
    }
    applyAlignment();
}

void Label::applyAlignment() {
    if (alignment_ == TextAlignment::Left) {
        sfText_.setOrigin(0.f, 0.f);
        sfText_.setPosition(getPadding().left, getPadding().top);
        return;
    }
    sf::FloatRect bounds = sfText_.getLocalBounds();
    float x = getPadding().left;
    float areaWidth = getSize().x - getPadding().left - getPadding().right;
    if (alignment_ == TextAlignment::Center) {
        x = getPadding().left + (areaWidth - bounds.width) / 2.f;
    } else if (alignment_ == TextAlignment::Right) {
        x = getSize().x - getPadding().right - bounds.width;
    }
    sfText_.setPosition(x, getPadding().top);
}

std::vector<std::string> Label::wrapText(const std::string& input, float maxWidth) const {
    std::vector<std::string> lines;
    std::istringstream stream(input);
    std::string word;
    std::string currentLine;

    while (stream >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        sf::Text test(testLine, *sfText_.getFont(), fontSize_);
        if (test.getLocalBounds().width > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

} // namespace engine::ui
