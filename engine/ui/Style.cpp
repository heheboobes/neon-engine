#include "Style.h"

namespace engine::ui {

bool Style::initialized_ = false;
Style Style::default_;
Style Style::dark_;
Style Style::light_;

Style::Style() {
    colors_.normal = sf::Color(60, 60, 60, 255);
    colors_.hover = sf::Color(80, 80, 80, 255);
    colors_.pressed = sf::Color(40, 40, 40, 255);
    colors_.disabled = sf::Color(100, 100, 100, 128);
    colors_.background = sf::Color(35, 35, 38, 255);
    colors_.surface = sf::Color(45, 45, 48, 255);
    colors_.accent = sf::Color(80, 140, 220, 255);
    colors_.text = sf::Color(220, 220, 220, 255);
    colors_.textDisabled = sf::Color(120, 120, 120, 255);

    textStyle_.color = colors_.text;
    textStyle_.disabledColor = colors_.textDisabled;
    textStyle_.fontSize = 14u;
    textStyle_.lineSpacing = 1.2f;

    borderStyle_.thickness = 1.f;
    borderStyle_.color = sf::Color(80, 80, 85, 255);
    borderStyle_.radius = 4.f;
}

Style& Style::getDefault() {
    if (!initialized_) {
        default_ = Style();
        dark_ = Style();
        dark_.colors_.background = sf::Color(25, 25, 28, 255);
        dark_.colors_.surface = sf::Color(35, 35, 38, 255);
        dark_.colors_.normal = sf::Color(50, 50, 53, 255);
        dark_.colors_.text = sf::Color(200, 200, 200, 255);

        light_ = Style();
        light_.colors_.background = sf::Color(240, 240, 242, 255);
        light_.colors_.surface = sf::Color(255, 255, 255, 255);
        light_.colors_.normal = sf::Color(220, 220, 225, 255);
        light_.colors_.hover = sf::Color(200, 200, 205, 255);
        light_.colors_.pressed = sf::Color(180, 180, 185, 255);
        light_.colors_.accent = sf::Color(50, 120, 200, 255);
        light_.colors_.text = sf::Color(30, 30, 35, 255);
        light_.colors_.textDisabled = sf::Color(150, 150, 155, 255);
        light_.borderStyle_.color = sf::Color(180, 180, 185, 255);

        initialized_ = true;
    }
    return default_;
}

Style& Style::getDark() {
    getDefault();
    return dark_;
}

Style& Style::getLight() {
    getDefault();
    return light_;
}

void Style::setFont(const sf::Font& font) {
    font_ = &font;
}

const sf::Font* Style::getFont() const {
    return font_;
}

void Style::setColors(const WidgetColors& colors) {
    colors_ = colors;
}

const WidgetColors& Style::getColors() const {
    return colors_;
}

void Style::setTextStyle(const TextStyle& style) {
    textStyle_ = style;
}

const TextStyle& Style::getTextStyle() const {
    return textStyle_;
}

void Style::setBorderStyle(const BorderStyle& style) {
    borderStyle_ = style;
}

const BorderStyle& Style::getBorderStyle() const {
    return borderStyle_;
}

PaddingPreset Style::getPaddingPreset() const {
    return paddingPreset_;
}

void Style::setGlobalScale(float scale) {
    globalScale_ = std::max(0.5f, std::min(scale, 3.0f));
    textStyle_.fontSize = static_cast<unsigned int>(14u * globalScale_);
    borderStyle_.thickness = std::max(0.5f, 1.f * globalScale_);
    borderStyle_.radius = 4.f * globalScale_;
    paddingPreset_.xs = 2.f * globalScale_;
    paddingPreset_.sm = 4.f * globalScale_;
    paddingPreset_.md = 8.f * globalScale_;
    paddingPreset_.lg = 12.f * globalScale_;
    paddingPreset_.xl = 16.f * globalScale_;
}

float Style::getGlobalScale() const {
    return globalScale_;
}

void Style::merge(const Style& other) {
    if (other.font_) font_ = other.font_;
    if (other.colors_.normal != sf::Color(60, 60, 60, 255)) colors_.normal = other.colors_.normal;
    if (other.colors_.hover != sf::Color(80, 80, 80, 255)) colors_.hover = other.colors_.hover;
    if (other.colors_.pressed != sf::Color(40, 40, 40, 255)) colors_.pressed = other.colors_.pressed;
    if (other.colors_.accent != sf::Color(80, 140, 220, 255)) colors_.accent = other.colors_.accent;
    textStyle_ = other.textStyle_;
    borderStyle_ = other.borderStyle_;
}

Style Style::clone() const {
    Style s;
    s.font_ = font_;
    s.colors_ = colors_;
    s.textStyle_ = textStyle_;
    s.borderStyle_ = borderStyle_;
    s.paddingPreset_ = paddingPreset_;
    s.globalScale_ = globalScale_;
    return s;
}

} // namespace engine::ui
