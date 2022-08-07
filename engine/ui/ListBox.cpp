#include "ListBox.h"
#include <algorithm>

namespace engine::ui {

ListBox::ListBox()
    : Widget()
{
    setSize({200.f, 150.f});
    background_.setSize(getSize());
    background_.setFillColor(backgroundColor_);
    selectionHighlight_.setFillColor(selectionColor_);
    hoverHighlight_.setFillColor(hoverColor_);
}

void ListBox::update(float dt) {
    Widget::update(dt);
}

bool ListBox::handleEvent(const sf::Event& event) {
    if (!isEnabled() || getVisibility() != Visibility::Visible) return false;

    sf::Vector2f mouse(0.f, 0.f);
    if (event.type == sf::Event::MouseMoved) {
        mouse = sf::Vector2f(static_cast<float>(event.mouseMove.x),
                             static_cast<float>(event.mouseMove.y));
    } else if (event.type == sf::Event::MouseButtonPressed) {
        mouse = sf::Vector2f(static_cast<float>(event.mouseButton.x),
                             static_cast<float>(event.mouseButton.y));
    }

    sf::Vector2f localPos = mouse - getPosition();

    if (event.type == sf::Event::MouseMoved) {
        size_t idx = indexAtPosition(localPos);
        hoveredIndex_ = idx;
        return true;
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button != sf::Mouse::Left) return false;
        size_t idx = indexAtPosition(localPos);
        if (idx >= items_.size()) return false;

        if (multiSelect_) {
            items_[idx].selected = !items_[idx].selected;
        } else {
            for (auto& item : items_) item.selected = false;
            items_[idx].selected = true;
        }

        if (onSelectionChanged_) {
            onSelectionChanged_(getSelectedIndices());
        }
        if (onChange) onChange(*this);
        return true;
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        float delta = event.mouseWheelScroll.delta;
        scrollOffset_ -= delta * itemHeight_ * 3.f;
        float maxScroll = std::max(0.f, items_.size() * itemHeight_ - getSize().y);
        scrollOffset_ = std::clamp(scrollOffset_, 0.f, maxScroll);
        return true;
    }

    return false;
}

void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());

    target.draw(background_, states);

    sf::RectangleShape clipRect(sf::Vector2f(getSize().x, getSize().y));
    sf::View originalView = target.getView();
    sf::View clipView(originalView);
    clipView.setViewport(sf::FloatRect(
        originalView.getViewport().left,
        originalView.getViewport().top,
        originalView.getViewport().width,
        originalView.getViewport().height
    ));
    target.setView(originalView);

    for (size_t i = 0; i < items_.size(); ++i) {
        float yPos = static_cast<float>(i) * itemHeight_ - scrollOffset_;
        if (yPos + itemHeight_ < 0.f || yPos > getSize().y) continue;

        if (items_[i].selected) {
            selectionHighlight_.setPosition(0.f, yPos);
            selectionHighlight_.setSize({getSize().x, itemHeight_});
            target.draw(selectionHighlight_, states);
        } else if (i == hoveredIndex_) {
            hoverHighlight_.setPosition(0.f, yPos);
            hoverHighlight_.setSize({getSize().x, itemHeight_});
            target.draw(hoverHighlight_, states);
        }

        if (i < textCache_.size()) {
            sf::Text t = textCache_[i];
            t.setPosition(4.f, yPos + 2.f);
            target.draw(t, states);
        }
    }

    Widget::draw(target, states);
}

void ListBox::addItem(const std::string& text, const std::string& value) {
    items_.emplace_back(text, value);
    rebuildTextCache();
}

void ListBox::insertItem(size_t index, const std::string& text, const std::string& value) {
    if (index > items_.size()) index = items_.size();
    items_.insert(items_.begin() + static_cast<ptrdiff_t>(index), ListItem(text, value));
    rebuildTextCache();
}

void ListBox::removeItem(size_t index) {
    if (index >= items_.size()) return;
    items_.erase(items_.begin() + static_cast<ptrdiff_t>(index));
    rebuildTextCache();
}

void ListBox::clearItems() {
    items_.clear();
    rebuildTextCache();
}

size_t ListBox::getItemCount() const {
    return items_.size();
}

const ListItem& ListBox::getItem(size_t index) const {
    return items_.at(index);
}

void ListBox::setItemText(size_t index, const std::string& text) {
    if (index >= items_.size()) return;
    items_[index].text = text;
    rebuildTextCache();
}

void ListBox::setMultiSelect(bool multi) {
    multiSelect_ = multi;
    if (!multi) {
        bool found = false;
        for (auto& item : items_) {
            if (found) item.selected = false;
            if (item.selected) found = true;
        }
    }
}

bool ListBox::isMultiSelect() const {
    return multiSelect_;
}

void ListBox::selectItem(size_t index) {
    if (index >= items_.size()) return;
    if (!multiSelect_) {
        for (auto& item : items_) item.selected = false;
    }
    items_[index].selected = true;
    if (onSelectionChanged_) onSelectionChanged_(getSelectedIndices());
}

void ListBox::deselectItem(size_t index) {
    if (index >= items_.size()) return;
    items_[index].selected = false;
    if (onSelectionChanged_) onSelectionChanged_(getSelectedIndices());
}

void ListBox::selectAll() {
    if (!multiSelect_) return;
    for (auto& item : items_) item.selected = true;
    if (onSelectionChanged_) onSelectionChanged_(getSelectedIndices());
}

void ListBox::deselectAll() {
    for (auto& item : items_) item.selected = false;
    if (onSelectionChanged_) onSelectionChanged_(getSelectedIndices());
}

std::vector<size_t> ListBox::getSelectedIndices() const {
    std::vector<size_t> result;
    for (size_t i = 0; i < items_.size(); ++i) {
        if (items_[i].selected) result.push_back(i);
    }
    return result;
}

std::vector<std::string> ListBox::getSelectedTexts() const {
    std::vector<std::string> result;
    for (const auto& item : items_) {
        if (item.selected) result.push_back(item.text);
    }
    return result;
}

void ListBox::setFont(const sf::Font& font) {
    for (auto& t : textCache_) t.setFont(font);
    rebuildTextCache();
}

void ListBox::setFontSize(unsigned int size) {
    fontSize_ = size;
    for (auto& t : textCache_) t.setCharacterSize(fontSize_);
    rebuildTextCache();
}

unsigned int ListBox::getFontSize() const {
    return fontSize_;
}

void ListBox::setItemHeight(float height) {
    itemHeight_ = height;
}

float ListBox::getItemHeight() const {
    return itemHeight_;
}

void ListBox::setTextColor(sf::Color color) {
    textColor_ = color;
    for (auto& t : textCache_) t.setFillColor(textColor_);
}

void ListBox::setBackgroundColor(sf::Color color) {
    backgroundColor_ = color;
    background_.setFillColor(backgroundColor_);
}

void ListBox::setSelectionColor(sf::Color color) {
    selectionColor_ = color;
    selectionHighlight_.setFillColor(selectionColor_);
}

void ListBox::setHoverColor(sf::Color color) {
    hoverColor_ = color;
    hoverHighlight_.setFillColor(hoverColor_);
}

void ListBox::setOnSelectionChanged(SelectionCallback callback) {
    onSelectionChanged_ = std::move(callback);
}

void ListBox::rebuildTextCache() {
    textCache_.clear();
    textCache_.reserve(items_.size());
    for (const auto& item : items_) {
        sf::Text t;
        t.setString(item.text);
        t.setCharacterSize(fontSize_);
        t.setFillColor(textColor_);
        textCache_.push_back(t);
    }
}

size_t ListBox::indexAtPosition(sf::Vector2f localPos) const {
    if (localPos.y < 0.f || localPos.y > getSize().y) return static_cast<size_t>(-1);
    float y = localPos.y + scrollOffset_;
    size_t idx = static_cast<size_t>(y / itemHeight_);
    if (idx >= items_.size()) return static_cast<size_t>(-1);
    return idx;
}

void ListBox::ensureVisible(size_t index) {
    if (index >= items_.size()) return;
    float itemTop = static_cast<float>(index) * itemHeight_;
    float itemBottom = itemTop + itemHeight_;
    if (itemTop < scrollOffset_) {
        scrollOffset_ = itemTop;
    } else if (itemBottom > scrollOffset_ + getSize().y) {
        scrollOffset_ = itemBottom - getSize().y;
    }
}

} // namespace engine::ui

