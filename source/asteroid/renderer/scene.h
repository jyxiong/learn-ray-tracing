#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "asteroid/cuda/device_buffer.h"
#include "asteroid/renderer/scene_struct.h"

namespace Asteroid {

    struct Scene;

    struct SceneView {

        BufferView<Sphere> deviceSpheres;

        BufferView<Material> deviceMaterials;

        explicit SceneView(const Scene &scene);

    };

    struct Scene {

        std::vector<Sphere> spheres;

        std::vector<Material> materials;

        DeviceBuffer<Sphere> deviceSpheres;

        DeviceBuffer<Material> deviceMaterials;

        void UpdateDevice();

    };

}
