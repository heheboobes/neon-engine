#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace engine::ui {

struct BorderStyle {
    float thickness = 1.f;
    sf::Color color{80, 80, 85, 255};
    float radius = 4.f;
};

struct TextStyle {
    sf::Color color{220, 220, 220, 255};
    sf::Color disabledColor{120, 120, 120, 255};
    unsigned int fontSize = 14u;
    float lineSpacing = 1.2f;
};

struct WidgetColors {
    sf::Color normal{60, 60, 60, 255};
    sf::Color hover{80, 80, 80, 255};
    sf::Color pressed{40, 40, 40, 255};
    sf::Color disabled{100, 100, 100, 128};
    sf::Color background{35, 35, 38, 255};
    sf::Color surface{45, 45, 48, 255};
    sf::Color accent{80, 140, 220, 255};
    sf::Color text{220, 220, 220, 255};
    sf::Color textDisabled{120, 120, 120, 255};
};

struct PaddingPreset {
    float xs = 2.f;
    float sm = 4.f;
    float md = 8.f;
    float lg = 12.f;
    float xl = 16.f;
};

class Style {
public:
    static Style& getDefault();
    static Style& getDark();
    static Style& getLight();

    void setFont(const sf::Font& font);
    const sf::Font* getFont() const;

    void setColors(const WidgetColors& colors);
    const WidgetColors& getColors() const;

    void setTextStyle(const TextStyle& style);
    const TextStyle& getTextStyle() const;

    void setBorderStyle(const BorderStyle& style);
    const BorderStyle& getBorderStyle() const;

    PaddingPreset getPaddingPreset() const;

    void setGlobalScale(float scale);
    float getGlobalScale() const;

    void merge(const Style& other);
    Style clone() const;

private:
    Style();

    const sf::Font* font_ = nullptr;
    WidgetColors colors_;
    TextStyle textStyle_;
    BorderStyle borderStyle_;
    PaddingPreset paddingPreset_;
    float globalScale_ = 1.f;

    static Style default_;
    static Style dark_;
    static Style light_;
    static bool initialized_;
};

} // namespace engine::ui
