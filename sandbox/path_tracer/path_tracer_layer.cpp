#include "path_tracer_layer.h"

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "asteroid/app/application.h"
#include "asteroid/util/timer.h"

using namespace Asteroid;

PathTracerLayer::PathTracerLayer() : Layer("Example")
{

    Material& pinkGeometry = m_scene.materials.emplace_back();
    pinkGeometry.albedo = { 1.0f, 0.0f, 1.0f };
    pinkGeometry.roughness = 0.0f;

    Material& blueGeometry = m_scene.materials.emplace_back();
    blueGeometry.albedo = { 0.2f, 0.3f, 1.0f };
    blueGeometry.roughness = 0.1f;

    Material& orangeGeometry = m_scene.materials.emplace_back();
    orangeGeometry.albedo = { 0.8f, 0.5f, 0.2f };
    orangeGeometry.roughness = 0.1f;
    orangeGeometry.emission = glm::vec3(1);

    {
        Geometry geometry;
        geometry.type = GeometryType::Sphere;
        geometry.materialIndex = 0;
        geometry.updateTransform();
        m_scene.geometries.push_back(geometry);
    }

    {
        Geometry geometry;
        geometry.type = GeometryType::Sphere;
        geometry.translation = { 2.0f, 0.0f, 0.0f };
        geometry.updateTransform();
        geometry.materialIndex = 2;
        m_scene.geometries.push_back(geometry);
    }

    {
        Geometry geometry;
        geometry.type = GeometryType::Sphere;
        geometry.translation = { 0.0f, -101.0f, 0.0f };
        geometry.scale = { 100.0f, 100.0f, 100.0f };
        geometry.updateTransform();
        geometry.materialIndex = 1;
        m_scene.geometries.push_back(geometry);
    }

    {
        Geometry geometry;
        geometry.type = GeometryType::Cube;
        geometry.translation = { -1.0f, 1.0f, 0.0f };
        geometry.scale = { 0.5f, 0.5f, 0.5f };
        geometry.updateTransform();
        geometry.materialIndex = 1;
        m_scene.geometries.push_back(geometry);
    }

    {
        AreaLight light;
        light.type = LightType::Square;
        light.emission = { 1.0f, 1.0f, 1.0f };
        light.translation = { 2.0f, 2.0f, 2.0f };
        light.update();
        m_scene.areaLights.push_back(light);
    }
    m_modified = true;
}

PathTracerLayer::~PathTracerLayer() = default;

void PathTracerLayer::OnAttach()
{
}

void PathTracerLayer::OnUpdate(float ts)
{
    m_modified |= m_cameraController.OnUpdate(ts);
}

void PathTracerLayer::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::Text("Last render: %.3fms", m_lastRenderTime);

    ImGui::Text("Current frame: %d", m_renderer.getRenderState().frame);

    m_modified |=
        ImGui::DragInt("Trace depth: %d", reinterpret_cast<int*>(&m_renderer.getRenderState().maxDepth), 1, 1, 100);

    m_modified |= ImGui::DragInt("Samples per pixel: %d",
                                 reinterpret_cast<int*>(&m_renderer.getRenderState().maxSamples),
                                 1,
                                 1,
                                 100);

    m_modified |= ImGui::Button("Reset");

    ImGui::End();

    ImGui::Begin("Scene");
    for (size_t i = 0; i < m_scene.geometries.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));

        Geometry& geometry = m_scene.geometries[i];

        m_modified |= ImGui::DragFloat3("translation", glm::value_ptr(geometry.translation), 0.1f);
        m_modified |= ImGui::DragFloat3("rotation", glm::value_ptr(geometry.rotation), 0.1f);
        m_modified |= ImGui::DragFloat3("scale", glm::value_ptr(geometry.scale), 0.1f);
        m_modified |= ImGui::DragInt("material ID", &geometry.materialIndex, 1, 0, (int) m_scene.materials.size() - 1);

        ImGui::Separator();

        ImGui::PopID();
    }

    ImGui::End();

    ImGui::Begin("Lights");
    for (size_t i = 0; i < m_scene.areaLights.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));

        AreaLight& light = m_scene.areaLights[i];

        m_modified |= ImGui::DragInt("type", reinterpret_cast<int*>(&light.type), 1, 0, 2);
        m_modified |= ImGui::Checkbox("two sided", &light.twoSided);

        m_modified |= ImGui::DragFloat3("translation", glm::value_ptr(light.translation), 0.1f);
        m_modified |= ImGui::DragFloat3("rotation", glm::value_ptr(light.rotation), 0.1f);
        m_modified |= ImGui::DragFloat3("scale", glm::value_ptr(light.scale), 0.1f);
        m_modified |= ImGui::DragFloat3("emission", glm::value_ptr(light.emission), 0.1f);

        ImGui::Separator();

        ImGui::PopID();
    }

    ImGui::End();

    ImGui::Begin("Material");

    for (size_t i = 0; i < m_scene.materials.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));

        Material& material = m_scene.materials[i];

        m_modified |= ImGui::ColorEdit3("albedo", glm::value_ptr(material.albedo));
        m_modified |= ImGui::DragFloat("roughness", &material.roughness, 0.01f, 0.0f, 1.0f);
        m_modified |= ImGui::DragFloat("metallic", &material.metallic, 0.01f, 0.0f, 1.0f);
        m_modified |= ImGui::DragFloat3("emission", glm::value_ptr(material.emission), 0.05f, 0.0f, FLT_MAX);

        ImGui::Separator();

        ImGui::PopID();
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    glm::ivec2 viewport = glm::ivec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    if (viewport != m_viewport)
    {
        m_viewport = viewport;
        m_resized = true;
    }

    auto image = m_renderer.getFinalImage();
    if (image)
        ImGui::Image((void*) (intptr_t) image->rendererID(),
                     { (float) image->width(), (float) image->height() },
                     ImVec2(0, 1),
                     ImVec2(1, 0));

    ImGui::End();
    ImGui::PopStyleVar();

    render();
}

void PathTracerLayer::render()
{
    Timer timer;

    if (m_modified)
    {
        m_scene.updateDevice();

        m_renderer.resetFrameIndex();

        m_modified = false;
    }

    if (m_resized)
    {
        m_cameraController.onResize(m_viewport);
        m_renderer.onResize(m_viewport);

        m_resized = false;
    }

    m_renderer.render(m_scene, m_cameraController.GetCamera());

    m_lastRenderTime = timer.elapsedMillis();
}
