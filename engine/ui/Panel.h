#pragma once

#include "Widget.h"

namespace engine::ui {

enum class LayoutMode {
    Vertical,
    Horizontal,
    Grid
};

class Panel : public Widget {
public:
    Panel();
    explicit Panel(LayoutMode layoutMode);
    ~Panel() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const;

    void setBackgroundColor(sf::Color color);
    sf::Color getBackgroundColor() const;

    void setBorderColor(sf::Color color);
    sf::Color getBorderColor() const;

    void setBorderThickness(float thickness);
    float getBorderThickness() const;

    void setGridColumns(int columns);
    int getGridColumns() const;

    void setSpacing(float spacing);
    float getSpacing() const;

    void setPadding(float all);
    void setPadding(float horizontal, float vertical);

private:
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    LayoutMode layoutMode_ = LayoutMode::Vertical;
    sf::Color backgroundColor_{45, 45, 48, 255};
    sf::Color borderColor_{80, 80, 85, 255};
    float borderThickness_ = 1.f;
    int gridColumns_ = 2;
    float spacing_ = 4.f;

    void arrangeChildren();
    void arrangeVertical();
    void arrangeHorizontal();
    void arrangeGrid();
};

} // namespace engine::ui
