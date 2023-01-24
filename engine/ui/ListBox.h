#pragma once

#include "Widget.h"
#include <vector>
#include <string>

namespace engine::ui {

struct ListItem {
    std::string text;
    std::string value;
    bool selected = false;

    ListItem() = default;
    ListItem(const std::string& t, const std::string& v = "")
        : text(t), value(v) {}
};

class ListBox : public Widget {
public:
    using SelectionCallback = std::function<void(const std::vector<size_t>&)>;

    ListBox();
    ~ListBox() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void addItem(const std::string& text, const std::string& value = "");
    void insertItem(size_t index, const std::string& text, const std::string& value = "");
    void removeItem(size_t index);
    void clearItems();
    size_t getItemCount() const;

    const ListItem& getItem(size_t index) const;
    void setItemText(size_t index, const std::string& text);

    void setMultiSelect(bool multi);
    bool isMultiSelect() const;

    void selectItem(size_t index);
    void deselectItem(size_t index);
    void selectAll();
    void deselectAll();

    std::vector<size_t> getSelectedIndices() const;
    std::vector<std::string> getSelectedTexts() const;

    void setFont(const sf::Font& font);
    void setFontSize(unsigned int size);
    unsigned int getFontSize() const;

    void setItemHeight(float height);
    float getItemHeight() const;

    void setTextColor(sf::Color color);
    void setBackgroundColor(sf::Color color);
    void setSelectionColor(sf::Color color);
    void setHoverColor(sf::Color color);

    void setOnSelectionChanged(SelectionCallback callback);

private:
    std::vector<ListItem> items_;
    size_t hoveredIndex_ = static_cast<size_t>(-1);
    float scrollOffset_ = 0.f;
    bool multiSelect_ = false;

    sf::RectangleShape background_;
    sf::RectangleShape selectionHighlight_;
    sf::RectangleShape hoverHighlight_;
    std::vector<sf::Text> textCache_;

    sf::Color textColor_{200, 200, 200, 255};
    sf::Color backgroundColor_{30, 30, 33, 255};
    sf::Color selectionColor_{60, 100, 180, 180};
    sf::Color hoverColor_{60, 60, 65, 180};
    unsigned int fontSize_ = 14u;
    float itemHeight_ = 22.f;

    SelectionCallback onSelectionChanged_;

    void rebuildTextCache();
    size_t indexAtPosition(sf::Vector2f localPos) const;
    void ensureVisible(size_t index);
};

} // namespace engine::ui
