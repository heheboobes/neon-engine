#pragma once

#include <engine/ecs/System.h>
#include <engine/ecs/Entity.h>
#include <string>
#include <vector>
#include <functional>

namespace examples::rpg
{

enum class ItemType
{
    Consumable,
    Weapon,
    Armor,
    KeyItem,
    QuestItem
};

struct Item
{
    int id;
    std::string name;
    std::string description;
    ItemType type;
    int value;
    int stackSize;
    int maxStack;
    std::function<void(engine::ecs::Entity& user)> onUse;
};

struct InventoryComponent
{
    std::vector<Item> items;
    int gold;
    int capacity;
    int equippedWeaponIndex;
    int equippedArmorIndex;
};

class InventorySystem : public engine::ecs::System
{
public:
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    bool addItem(engine::ecs::Entity& entity, const Item& item);
    bool removeItem(engine::ecs::Entity& entity, int itemId, int count = 1);
    bool useItem(engine::ecs::Entity& entity, int slotIndex);
    bool equipItem(engine::ecs::Entity& entity, int slotIndex);
    bool hasItem(engine::ecs::Entity& entity, int itemId) const;
    int getItemCount(engine::ecs::Entity& entity, int itemId) const;

    static Item createPotion(int healAmount);
    static Item createSword(int damage, const std::string& name);
    static Item createShield(int defense, const std::string& name);

private:
    int findEmptySlot(InventoryComponent& inv) const;
    void sortInventory(InventoryComponent& inv);
};

}
