#ifndef ENGINE_ECS_COMPONENT_MASK_H
#define ENGINE_ECS_COMPONENT_MASK_H

#include "Component.h"
#include <bitset>
#include <cstdint>
#include <string>

namespace engine {
namespace ecs {

class ComponentMask {
public:
    ComponentMask() = default;

    void SetBit(ComponentTypeID id, bool value = true)
    {
        if (id < MAX_COMPONENT_TYPES)
            m_bits.set(id, value);
    }

    bool GetBit(ComponentTypeID id) const
    {
        if (id < MAX_COMPONENT_TYPES)
            return m_bits.test(id);
        return false;
    }

    void Clear() { m_bits.reset(); }
    void SetAll() { m_bits.set(); }
    bool Any() const { return m_bits.any(); }
    bool None() const { return m_bits.none(); }
    bool AllBitsSet() const { return m_bits.all(); }
    size_t Count() const { return m_bits.count(); }

    ComponentMask operator&(const ComponentMask& other) const
    {
        ComponentMask result;
        result.m_bits = m_bits & other.m_bits;
        return result;
    }

    ComponentMask operator|(const ComponentMask& other) const
    {
        ComponentMask result;
        result.m_bits = m_bits | other.m_bits;
        return result;
    }

    ComponentMask operator~() const
    {
        ComponentMask result;
        result.m_bits = ~m_bits;
        return result;
    }

    ComponentMask& operator&=(const ComponentMask& other)
    {
        m_bits &= other.m_bits;
        return *this;
    }

    ComponentMask& operator|=(const ComponentMask& other)
    {
        m_bits |= other.m_bits;
        return *this;
    }

    bool operator==(const ComponentMask& other) const
    {
        return m_bits == other.m_bits;
    }

    bool operator!=(const ComponentMask& other) const
    {
        return m_bits != other.m_bits;
    }

    bool IsSubsetOf(const ComponentMask& other) const
    {
        return (m_bits & other.m_bits) == m_bits;
    }

    bool Intersects(const ComponentMask& other) const
    {
        return (m_bits & other.m_bits).any();
    }

    std::string ToString() const;

    template<typename... Components>
    void SetTypes()
    {
        ((m_bits.set(Components::GetStaticTypeID())), ...);
    }

    template<typename... Components>
    static ComponentMask Create()
    {
        ComponentMask mask;
        mask.SetTypes<Components...>();
        return mask;
    }

    static ComponentMask All()
    {
        ComponentMask mask;
        mask.SetAll();
        return mask;
    }

private:
    std::bitset<MAX_COMPONENT_TYPES> m_bits;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_COMPONENT_MASK_H
