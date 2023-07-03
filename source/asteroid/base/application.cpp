#include <memory>

#include "asteroid/base/application.h"
#include "asteroid/event/application_event.h"
#include "asteroid/util/log.h"
#include "asteroid/util/macro.h"

using namespace Asteroid;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    AST_CORE_ASSERT(!s_Instance, "Application already exists!")
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create());

    m_Window->SetEventCallback([this](Event &e) -> void {
        OnEvent(e);
    });

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application() = default;

void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}


void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &e) -> bool {
        return OnWindowClose(e);
    });

    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent &e) -> bool {
        return OnWindowResize(e);
    });

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}

void Application::Run()
{
    while (m_Running)
    {
        if (!m_Minimized)
        {
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();
        }

        m_ImGuiLayer->Begin();
        for (Layer *layer: m_LayerStack)
            layer->OnImGuiRender();
        m_ImGuiLayer->End();

        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent &e)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    glViewport(0, 0, e.GetWidth(), e.GetHeight());

    return false;
}
