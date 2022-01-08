#include "Application.h"
#include "Time.h"
#include "ServiceLocator.h"
#include <iostream>
#include <cassert>

namespace engine {
namespace core {

Application::Application()
{
}

Application::~Application()
{
    Shutdown();
}

Application& Application::Instance()
{
    static Application instance;
    return instance;
}

bool Application::Init()
{
    m_status = AppStatus::Initializing;
    m_config.Validate();

    CreateWindow();
    if (!m_window || !m_window->isOpen())
    {
        std::cerr << "Application::Init - Failed to create render window\n";
        m_status = AppStatus::Exited;
        return false;
    }

    m_gameLoop.SetFixedUpdateCallback(
        [this](float fixedDt) { OnFixedUpdate(fixedDt); });
    m_gameLoop.SetUpdateCallback(
        [this](float deltaTime) { OnUpdate(deltaTime); });
    m_gameLoop.SetRenderCallback(
        [this](float alpha) { OnRender(alpha); });

    m_gameLoop.SetFixedTimestep(m_config.physics.fixedTimestep);
    m_gameLoop.SetTargetFPS(m_config.video.maxFPS);
    m_gameLoop.SetMaxSubSteps(m_config.physics.maxSubSteps);

    Time::Instance().Reset();

    m_status = AppStatus::Running;
    return true;
}

void Application::CreateWindow()
{
    sf::Uint32 style = sf::Style::Default;
    if (m_config.video.fullscreen)
        style = sf::Style::Fullscreen;

    sf::ContextSettings context;
    context.antialiasingLevel = 0;
    context.majorVersion = 2;
    context.minorVersion = 1;

    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(m_config.video.width, m_config.video.height),
        m_config.rendering.windowTitle,
        style,
        context
    );

    m_window->setVerticalSyncEnabled(m_config.video.vsyncEnabled);
    m_window->setFramerateLimit(m_config.video.maxFPS);
    m_window->setKeyRepeatEnabled(false);
    m_window->setMouseCursorVisible(true);
}

int Application::Run()
{
    if (m_status == AppStatus::Uninitialized)
    {
        if (!Init())
            return -1;
    }

    while (m_window && m_window->isOpen() && m_status != AppStatus::Exited)
    {
        ProcessEvents();

        if (m_status == AppStatus::Running)
        {
            m_gameLoop.Tick();
        }
    }

    m_status = AppStatus::Exited;
    return 0;
}

void Application::Shutdown()
{
    if (m_status == AppStatus::Exited || m_status == AppStatus::Uninitialized)
        return;

    m_status = AppStatus::Stopping;

    ServiceLocator::ClearAll();

    if (m_window)
    {
        m_window->close();
        m_window.reset();
    }

    m_status = AppStatus::Exited;
}

void Application::ProcessEvents()
{
    sf::Event event;
    while (m_window->pollEvent(event))
    {
        HandleWindowEvents(event);
        OnEvent(event);
        m_layerStack.OnEvent(&event);
    }
}

void Application::HandleWindowEvents(sf::Event& event)
{
    switch (event.type)
    {
    case sf::Event::Closed:
        Shutdown();
        break;

    case sf::Event::Resized:
    {
        sf::FloatRect visibleArea(0, 0,
            static_cast<float>(event.size.width),
            static_cast<float>(event.size.height));
        m_window->setView(sf::View(visibleArea));
        m_config.video.width = event.size.width;
        m_config.video.height = event.size.height;
        break;
    }

    case sf::Event::LostFocus:
        m_status = AppStatus::Paused;
        break;

    case sf::Event::GainedFocus:
        if (m_status == AppStatus::Paused)
            m_status = AppStatus::Running;
        break;

    default:
        break;
    }
}

void Application::OnUpdate(float deltaTime)
{
    m_stateManager.Update(deltaTime);
    m_layerStack.OnUpdate(deltaTime);
}

void Application::OnFixedUpdate(float fixedDt)
{
    m_layerStack.OnFixedUpdate(fixedDt);
}

void Application::OnRender(float alpha)
{
    if (!m_window) return;

    m_window->clear(sf::Color(20, 20, 30));

    m_stateManager.Render(alpha);
    m_layerStack.OnRender(alpha);

    m_window->display();
}

void Application::OnEvent(sf::Event& event)
{
    (void)event;
}

} // namespace core
} // namespace engine
