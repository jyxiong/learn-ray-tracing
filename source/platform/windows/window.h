#pragma once

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "asteroid/core/window.h"

namespace Asteroid
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps &props);

    virtual ~WindowsWindow();

    void OnUpdate() override;

    inline unsigned int GetWidth() const override { return m_Data.Width; }

    inline unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    inline void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }

    void SetVSync(bool enabled) override;

    bool IsVSync() const override;

private:
    void Init(const WindowProps &props);

    void Shutdown();

private:
    GLFWwindow *m_Window;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};

}