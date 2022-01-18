#include "Layer.h"
#include <algorithm>
#include <cassert>

namespace engine {
namespace core {

LayerStack::~LayerStack()
{
    for (Layer* layer : m_layers)
    {
        layer->OnDetach();
        delete layer;
    }
    m_layers.clear();
}

void LayerStack::PushLayer(Layer* layer)
{
    m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
    m_layerInsertIndex++;
    layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay)
{
    m_layers.emplace_back(overlay);
    overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end() && it < m_layers.begin() + m_layerInsertIndex)
    {
        layer->OnDetach();
        m_layers.erase(it);
        m_layerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
    if (it != m_layers.end() && it >= m_layers.begin() + m_layerInsertIndex)
    {
        overlay->OnDetach();
        m_layers.erase(it);
    }
}

void LayerStack::OnUpdate(float deltaTime)
{
    for (Layer* layer : m_layers)
    {
        if (layer->IsEnabled())
            layer->OnUpdate(deltaTime);
    }
}

void LayerStack::OnFixedUpdate(float fixedDt)
{
    for (Layer* layer : m_layers)
    {
        if (layer->IsEnabled())
            layer->OnFixedUpdate(fixedDt);
    }
}

void LayerStack::OnRender(float alpha)
{
    for (Layer* layer : m_layers)
    {
        if (layer->IsEnabled())
            layer->OnRender(alpha);
    }
}

void LayerStack::OnEvent(void* event)
{
    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
    {
        if ((*it)->IsEnabled())
            (*it)->OnEvent(event);
    }
}

} // namespace core
} // namespace engine
