#ifndef ENGINE_CORE_LAYER_H
#define ENGINE_CORE_LAYER_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace engine {
namespace core {

class Layer {
public:
    Layer(const std::string& name = "Layer")
        : m_name(name), m_enabled(true) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnFixedUpdate(float fixedDt) {}
    virtual void OnRender(float alpha) {}
    virtual void OnEvent(void* event) {}

    const std::string& GetName() const { return m_name; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }

private:
    std::string m_name;
    bool m_enabled;
};

class LayerStack {
public:
    LayerStack() = default;
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);

    void OnUpdate(float deltaTime);
    void OnFixedUpdate(float fixedDt);
    void OnRender(float alpha);
    void OnEvent(void* event);

    using iterator = std::vector<Layer*>::iterator;
    using const_iterator = std::vector<Layer*>::const_iterator;

    iterator begin() { return m_layers.begin(); }
    iterator end() { return m_layers.end(); }
    const_iterator begin() const { return m_layers.begin(); }
    const_iterator end() const { return m_layers.end(); }

    size_t Size() const { return m_layers.size(); }
    Layer* operator[](size_t index) { return m_layers[index]; }

private:
    std::vector<Layer*> m_layers;
    uint32_t m_layerInsertIndex = 0;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_LAYER_H
