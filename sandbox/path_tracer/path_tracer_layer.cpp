#include "path_tracer_layer.h"

#include "glad/gl.h"
#include "cuda_gl_interop.h"
#include "imgui.h"
#include "glm/glm.hpp"

#include "asteroid/base/application.h"
#include "asteroid/util/helper_cuda.h"

using namespace Asteroid;

extern "C" void launch_cudaProcess(dim3 grid, dim3 block, int sbytes,
    glm::u8vec4* g_odata, int imgw);

ExampleLayer::ExampleLayer()
    : Layer("Example")
{
}

ExampleLayer::~ExampleLayer()
{
}

void ExampleLayer::OnUpdate()
{
    
}

void ExampleLayer::OnImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    auto image = m_Renderer.GetFinalImage();
    if (image)
        ImGui::Image((void*)(intptr_t)image->GetRendererID(), { (float)image->GetWidth(), (float)image->GetHeight() },
            ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
    ImGui::PopStyleVar();

    Render();
}

void ExampleLayer::OnEvent(Event &event)
{
}

void ExampleLayer::Render()
{
    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render();
}
