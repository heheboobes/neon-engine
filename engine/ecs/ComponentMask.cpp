#include "ComponentMask.h"
#include <sstream>

namespace engine {
namespace ecs {

std::string ComponentMask::ToString() const
{
    std::ostringstream ss;
    ss << "ComponentMask[";
    bool first = true;
    for (ComponentTypeID i = 0; i < MAX_COMPONENT_TYPES; ++i)
    {
        if (m_bits.test(i))
        {
            if (!first) ss << ", ";
            ss << i;
            first = false;
        }
    }
    ss << "]";
    return ss.str();
}

} // namespace ecs
} // namespace engine
