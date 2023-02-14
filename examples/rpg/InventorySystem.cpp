#include "InventorySystem.h"
#include <engine/graphics/SpriteComponent.h>
#include <algorithm>
#include <iostream>

namespace examples::rpg
{

void InventorySystem::update(float dt)
{
    for (auto& entity : getEntities())
    {
        auto* inv = entity.getComponent<InventoryComponent>();
        if (!inv) continue;

        for (auto it = inv->items.begin(); it != inv->items.end(); )
        {
            if (it->stackSize <= 0)
                it = inv->items.erase(it);
            else
                ++it;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        {
            sortInventory(*inv);
        }
    }
}

void InventorySystem::render(sf::RenderWindow& window)
{
}

bool InventorySystem::addItem(engine::ecs::Entity& entity, const Item& item)
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return false;

    for (auto& existing : inv->items)
    {
        if (existing.id == item.id && existing.stackSize < existing.maxStack)
        {
            int space = existing.maxStack - existing.stackSize;
            int toAdd = std::min(item.stackSize, space);
            existing.stackSize += toAdd;
            return true;
        }
    }

    if (static_cast<int>(inv->items.size()) >= inv->capacity)
    {
        std::cerr << "Inventory full!\n";
        return false;
    }

    inv->items.push_back(item);
    return true;
}

bool InventorySystem::removeItem(engine::ecs::Entity& entity, int itemId, int count)
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return false;

    for (auto& existing : inv->items)
    {
        if (existing.id == itemId && existing.stackSize >= count)
        {
            existing.stackSize -= count;
            return true;
        }
    }
    return false;
}

bool InventorySystem::useItem(engine::ecs::Entity& entity, int slotIndex)
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return false;
    if (slotIndex < 0 || slotIndex >= static_cast<int>(inv->items.size()))
        return false;

    Item& item = inv->items[slotIndex];
    if (item.onUse)
    {
        item.onUse(entity);
        item.stackSize--;
        return true;
    }
    return false;
}

bool InventorySystem::equipItem(engine::ecs::Entity& entity, int slotIndex)
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return false;
    if (slotIndex < 0 || slotIndex >= static_cast<int>(inv->items.size()))
        return false;

    Item& item = inv->items[slotIndex];
    if (item.type == ItemType::Weapon)
    {
        inv->equippedWeaponIndex = slotIndex;
        return true;
    }
    if (item.type == ItemType::Armor)
    {
        inv->equippedArmorIndex = slotIndex;
        return true;
    }
    return false;
}

bool InventorySystem::hasItem(engine::ecs::Entity& entity, int itemId) const
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return false;

    for (const auto& item : inv->items)
    {
        if (item.id == itemId && item.stackSize > 0)
            return true;
    }
    return false;
}

int InventorySystem::getItemCount(engine::ecs::Entity& entity, int itemId) const
{
    auto* inv = entity.getComponent<InventoryComponent>();
    if (!inv) return 0;

    int total = 0;
    for (const auto& item : inv->items)
    {
        if (item.id == itemId)
            total += item.stackSize;
    }
    return total;
}

int InventorySystem::findEmptySlot(InventoryComponent& inv) const
{
    for (int i = 0; i < inv.capacity; ++i)
    {
        bool occupied = false;
        for (const auto& item : inv.items)
        {
            if (item.id == i) { occupied = true; break; }
        }
        if (!occupied) return i;
    }
    return -1;
}

void InventorySystem::sortInventory(InventoryComponent& inv)
{
    std::sort(inv.items.begin(), inv.items.end(),
        [](const Item& a, const Item& b) {
            if (a.type != b.type)
                return static_cast<int>(a.type) < static_cast<int>(b.type);
            return a.name < b.name;
        });
}

Item InventorySystem::createPotion(int healAmount)
{
    Item potion;
    potion.id = 1;
    potion.name = "Health Potion";
    potion.description = "Restores " + std::to_string(healAmount) + " HP";
    potion.type = ItemType::Consumable;
    potion.value = 50;
    potion.stackSize = 1;
    potion.maxStack = 10;
    potion.onUse = [healAmount](engine::ecs::Entity& user) {
        std::cout << "Healed for " << healAmount << " HP!\n";
    };
    return potion;
}

Item InventorySystem::createSword(int damage, const std::string& name)
{
    Item sword;
    sword.id = 10;
    sword.name = name;
    sword.description = "Deals " + std::to_string(damage) + " damage";
    sword.type = ItemType::Weapon;
    sword.value = 200;
    sword.stackSize = 1;
    sword.maxStack = 1;
    sword.onUse = [damage](engine::ecs::Entity& user) {
        std::cout << "Equipped sword with " << damage << " attack!\n";
    };
    return sword;
}

Item InventorySystem::createShield(int defense, const std::string& name)
{
    Item shield;
    shield.id = 20;
    shield.name = name;
    shield.description = "Provides " + std::to_string(defense) + " defense";
    shield.type = ItemType::Armor;
    shield.value = 150;
    shield.stackSize = 1;
    shield.maxStack = 1;
    shield.onUse = [defense](engine::ecs::Entity& user) {
        std::cout << "Equipped shield with " << defense << " defense!\n";
    };
    return shield;
}

}
