#include "TextBox.h"
#include <algorithm>
#include <cctype>

namespace engine::ui {

TextBox::TextBox()
    : Widget()
{
    setSize({200.f, 28.f});
    background_.setSize(getSize());
    background_.setFillColor(backgroundColor_);
    sfText_.setCharacterSize(fontSize_);
    sfText_.setFillColor(textColor_);
    placeholderText_.setCharacterSize(fontSize_);
    placeholderText_.setFillColor(placeholderColor_);
    cursor_.setSize({1.f, static_cast<float>(fontSize_) + 4.f});
    cursor_.setFillColor(cursorColor_);
}

TextBox::TextBox(const std::string& text)
    : Widget()
    , text_(text)
{
    setSize({200.f, 28.f});
    background_.setSize(getSize());
    background_.setFillColor(backgroundColor_);
    sfText_.setCharacterSize(fontSize_);
    sfText_.setFillColor(textColor_);
    placeholderText_.setCharacterSize(fontSize_);
    placeholderText_.setFillColor(placeholderColor_);
    cursor_.setSize({1.f, static_cast<float>(fontSize_) + 4.f});
    cursor_.setFillColor(cursorColor_);
    cursorPos_ = text_.length();
    updateDisplayString();
}

void TextBox::update(float dt) {
    Widget::update(dt);
    if (focused_) {
        cursorTimer_ += dt;
        if (cursorTimer_ >= 0.5f) {
            cursorTimer_ = 0.f;
            cursorVisible_ = !cursorVisible_;
        }
    } else {
        cursorVisible_ = false;
    }
}

bool TextBox::handleEvent(const sf::Event& event) {
    if (!isEnabled() || getVisibility() != Visibility::Visible) return false;

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        focused_ = contains(mouse);
        if (focused_) {
            clampCursor();
        }
        return focused_;
    }

    if (event.type == sf::Event::KeyPressed && focused_ && !readOnly_) {
        switch (event.key.code) {
            case sf::Keyboard::Left:
                if (cursorPos_ > 0) cursorPos_--;
                break;
            case sf::Keyboard::Right:
                if (cursorPos_ < text_.length()) cursorPos_++;
                break;
            case sf::Keyboard::Home:
                cursorPos_ = 0;
                break;
            case sf::Keyboard::End:
                cursorPos_ = text_.length();
                break;
            case sf::Keyboard::Delete:
                if (cursorPos_ < text_.length()) {
                    text_.erase(cursorPos_, 1);
                    if (onChange_) onChange_(text_);
                }
                break;
            case sf::Keyboard::Backspace:
                if (cursorPos_ > 0) {
                    cursorPos_--;
                    text_.erase(cursorPos_, 1);
                    if (onChange_) onChange_(text_);
                }
                break;
            case sf::Keyboard::V:
                if (event.key.control) {
                    // paste placeholder
                }
                break;
            default: break;
        }
        updateDisplayString();
        return true;
    }

    if (event.type == sf::Event::TextEntered && focused_ && !readOnly_) {
        char c = static_cast<char>(event.text.unicode);
        if (c >= 32 && c <= 126 && text_.length() < maxLength_) {
            insertChar(c);
            if (onChange_) onChange_(text_);
        }
        updateDisplayString();
        return true;
    }

    return false;
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());

    target.draw(background_, states);

    if (focused_ && selectionStart_ != selectionEnd_) {
        sf::FloatRect selBounds = sfText_.findCharacterPos(selectionStart_).x >
            sfText_.findCharacterPos(selectionEnd_).x
            ? sf::FloatRect(sfText_.findCharacterPos(selectionEnd_).x, 2.f,
                            sfText_.findCharacterPos(selectionStart_).x -
                            sfText_.findCharacterPos(selectionEnd_).x,
                            static_cast<float>(fontSize_) + 4.f)
            : sf::FloatRect(sfText_.findCharacterPos(selectionStart_).x, 2.f,
                            sfText_.findCharacterPos(selectionEnd_).x -
                            sfText_.findCharacterPos(selectionStart_).x,
                            static_cast<float>(fontSize_) + 4.f);
        sf::RectangleShape sel(sf::Vector2f(selBounds.width, selBounds.height));
        sel.setPosition(selBounds.left, selBounds.top);
        sel.setFillColor(selectionColor_);
        target.draw(sel, states);
    }

    if (text_.empty() && !focused_) {
        target.draw(placeholderText_, states);
    } else {
        target.draw(sfText_, states);
    }

    if (cursorVisible_ && focused_) {
        target.draw(cursor_, states);
    }

    Widget::draw(target, states);
}

void TextBox::setText(const std::string& text) {
    text_ = text;
    cursorPos_ = text_.length();
    clampCursor();
    updateDisplayString();
}

const std::string& TextBox::getText() const {
    return text_;
}

void TextBox::setFont(const sf::Font& font) {
    sfText_.setFont(font);
    placeholderText_.setFont(font);
    updateDisplayString();
}

void TextBox::setFontSize(unsigned int size) {
    fontSize_ = size;
    sfText_.setCharacterSize(fontSize_);
    placeholderText_.setCharacterSize(fontSize_);
    cursor_.setSize({1.f, static_cast<float>(fontSize_) + 4.f});
    updateDisplayString();
}

unsigned int TextBox::getFontSize() const {
    return fontSize_;
}

void TextBox::setTextColor(sf::Color color) {
    textColor_ = color;
    sfText_.setFillColor(textColor_);
}

void TextBox::setBackgroundColor(sf::Color color) {
    backgroundColor_ = color;
    background_.setFillColor(backgroundColor_);
}

void TextBox::setCursorColor(sf::Color color) {
    cursorColor_ = color;
    cursor_.setFillColor(cursorColor_);
}

void TextBox::setSelectionColor(sf::Color color) {
    selectionColor_ = color;
}

void TextBox::setMaxLength(size_t maxLength) {
    maxLength_ = maxLength;
    if (text_.length() > maxLength_) {
        text_ = text_.substr(0, maxLength_);
        clampCursor();
        updateDisplayString();
    }
}

size_t TextBox::getMaxLength() const {
    return maxLength_;
}

void TextBox::setPasswordMode(bool password) {
    passwordMode_ = password;
    updateDisplayString();
}

bool TextBox::isPasswordMode() const {
    return passwordMode_;
}

void TextBox::setPlaceholder(const std::string& text) {
    placeholder_ = text;
    placeholderText_.setString(placeholder_);
}

const std::string& TextBox::getPlaceholder() const {
    return placeholder_;
}

void TextBox::setReadOnly(bool readOnly) {
    readOnly_ = readOnly;
}

bool TextBox::isReadOnly() const {
    return readOnly_;
}

void TextBox::setOnChange(ChangeCallback callback) {
    onChange_ = std::move(callback);
}

void TextBox::setCursorPosition(size_t pos) {
    cursorPos_ = pos;
    clampCursor();
    updateCursorPosition();
}

size_t TextBox::getCursorPosition() const {
    return cursorPos_;
}

void TextBox::selectAll() {
    selectionStart_ = 0;
    selectionEnd_ = text_.length();
}

void TextBox::clearSelection() {
    selectionStart_ = 0;
    selectionEnd_ = 0;
}

void TextBox::insertChar(char c) {
    text_.insert(text_.begin() + static_cast<ptrdiff_t>(cursorPos_), c);
    cursorPos_++;
}

void TextBox::deleteChar() {
    if (cursorPos_ < text_.length()) {
        text_.erase(cursorPos_, 1);
    }
}

void TextBox::updateDisplayString() {
    if (passwordMode_) {
        std::string masked(text_.length(), '*');
        sfText_.setString(masked);
    } else {
        sfText_.setString(text_);
    }
    sfText_.setPosition(getPadding().left + 4.f, getPadding().top + 2.f);
    placeholderText_.setPosition(getPadding().left + 4.f, getPadding().top + 2.f);
    updateCursorPosition();
}

void TextBox::updateCursorPosition() {
    clampCursor();
    std::string displayStr = passwordMode_ ? std::string(text_.length(), '*') : text_;
    std::string beforeCursor = displayStr.substr(0, cursorPos_);
    sf::Text temp( beforeCursor, *sfText_.getFont(), fontSize_);
    float cursorX = getPadding().left + 4.f + temp.getLocalBounds().width;
    cursor_.setPosition(cursorX, getPadding().top + 2.f);
}

void TextBox::clampCursor() {
    cursorPos_ = std::min(cursorPos_, text_.length());
}

} // namespace engine::ui

