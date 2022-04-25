#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"

namespace engine {
namespace components {

enum class LightType {
    Point,
    Directional,
    Spot,
    Ambient
};

class LightComponent : public ecs::Component<LightComponent> {
public:
    sf::Color color;
    float radius;
    float intensity;
    float innerAngle;
    float outerAngle;
    float direction;
    float falloff;
    float softness;

    LightType type;
    bool castsShadow;
    bool enabled;
    bool volumetric;

    int shadowMapSize;
    float shadowBias;
    int shadowSamples;

    LightComponent();

    void SetColor(const sf::Color& col);
    void SetIntensity(float i);
    void SetRadius(float r);
    void SetAngle(float inner, float outer);
    void SetDirection(float deg);
    void SetFalloff(float f);
    void SetSoftness(float s);

    sf::Color GetFinalColor() const;

    void SetType(LightType t);
    LightType GetType() const;

    void EnableShadow(bool enable);
    void SetShadowMapSize(int size);
    void SetShadowBias(float bias);

    float GetBrightnessAtDistance(float dist) const;
    bool IsInRange(float dist) const;

    void SetVolumetric(bool vol);
    void Toggle();
};

} // namespace components
} // namespace engine
