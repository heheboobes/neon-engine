#include "LightComponent.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

LightComponent::LightComponent()
    : color(sf::Color::White)
    , radius(300.0f)
    , intensity(1.0f)
    , innerAngle(0.0f)
    , outerAngle(45.0f)
    , direction(0.0f)
    , falloff(1.0f)
    , softness(0.2f)
    , type(LightType::Point)
    , castsShadow(true)
    , enabled(true)
    , volumetric(false)
    , shadowMapSize(512)
    , shadowBias(0.005f)
    , shadowSamples(16)
{
}

void LightComponent::SetColor(const sf::Color& col)
{
    color = col;
}

void LightComponent::SetIntensity(float i)
{
    intensity = std::max(0.0f, i);
}

void LightComponent::SetRadius(float r)
{
    radius = std::max(0.0f, r);
}

void LightComponent::SetAngle(float inner, float outer)
{
    innerAngle = std::clamp(inner, 0.0f, outer);
    outerAngle = std::clamp(outer, innerAngle, 360.0f);
}

void LightComponent::SetDirection(float deg)
{
    direction = std::fmod(deg, 360.0f);
    if (direction < 0.0f) direction += 360.0f;
}

void LightComponent::SetFalloff(float f)
{
    falloff = std::max(0.0f, f);
}

void LightComponent::SetSoftness(float s)
{
    softness = std::clamp(s, 0.0f, 1.0f);
}

sf::Color LightComponent::GetFinalColor() const
{
    return sf::Color(
        static_cast<uint8_t>(std::min(255, static_cast<int>(color.r * intensity))),
        static_cast<uint8_t>(std::min(255, static_cast<int>(color.g * intensity))),
        static_cast<uint8_t>(std::min(255, static_cast<int>(color.b * intensity))),
        color.a
    );
}

void LightComponent::SetType(LightType t)
{
    type = t;
}

LightType LightComponent::GetType() const
{
    return type;
}

void LightComponent::EnableShadow(bool enable)
{
    castsShadow = enable;
}

void LightComponent::SetShadowMapSize(int size)
{
    shadowMapSize = std::max(64, std::min(4096, size));
}

void LightComponent::SetShadowBias(float bias)
{
    shadowBias = std::max(0.0f, bias);
}

float LightComponent::GetBrightnessAtDistance(float dist) const
{
    if (dist >= radius) return 0.0f;
    float normalized = dist / radius;
    float attenuation = 1.0f / (1.0f + falloff * normalized * normalized);
    return attenuation * intensity;
}

bool LightComponent::IsInRange(float dist) const
{
    return dist <= radius;
}

void LightComponent::SetVolumetric(bool vol)
{
    volumetric = vol;
}

void LightComponent::Toggle()
{
    enabled = !enabled;
}

} // namespace components
} // namespace engine
