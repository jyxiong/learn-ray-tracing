#include <iostream>
#include <memory>

#include "material.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "moving_sphere.h"
#include "bvh.h"

#include "Common/vec3.h"
#include "Common/ray.h"
#include "Common/color.h"
#include "Common/rtweekend.h"
#include "Common/camera.h"

// blend (1.0, 1.0, 1.0) and (0.5, 0.7, 1.0) with height or ray.y()
color ray_color(const ray &r, const hittable_list &world, int depth) {
    if (depth <= 0) // case 1: if not hit within depth recursive
        return {0.0, 0.0, 0.0};

    hit_record rec;

    // case 0: hit the world geometry
    // set t_min to 0.001 rather than 0.0 due to the floating point approximation.
    if (world.hit(r, 0.001, infinity, rec)) // step 1: record the hit information of input ray and world geometry
    {
        ray scattered;
        color attenuation;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) // step 2: get the scattered ray by record info
            return attenuation
                * ray_color(scattered, world, depth - 1); // step 3: blend the recursive color of the scattered ray
        return {0.0, 0.0, 0.0}; // case 2: if not scatter
    }

    // case 3: hit the sky
    // cause viewport height is 2.0 in range (-1.0, 1.0)
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, std::make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec3() * random_vec3();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(std::make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec3(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return static_cast<hittable_list>(std::make_shared<bvh_node>(world, 0, 1));
    //return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    objects.add(std::make_shared<sphere>(point3(0, -10, 0), 10, std::make_shared<lambertian>(checker)));
    objects.add(std::make_shared<sphere>(point3(0, 10, 0), 10, std::make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlin = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, std::make_shared<lambertian>(perlin)));
    objects.add(std::make_shared<sphere>(point3(0, 2, 0), 2, std::make_shared<lambertian>(perlin)));

    return objects;
}

hittable_list earth() {
    std::string image_path = "C:/Users/jyxiong/Projects/ray-tracing/Assets/earthmap.jpg";
    auto earth_texture = std::make_shared<image_texture>(image_path.c_str());
    auto earth_surface = std::make_shared<lambertian>(earth_texture);
    auto globe = std::make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

int main() {
    // image
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int depth = 50;

    // World
    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;

    switch (0) {
        case 1: {
            world = random_scene();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        }
        case 2: {
            world = two_spheres();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        }
        case 3: {
            world = two_perlin_spheres();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        }
        default:
        case 4: {
            world = earth();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        }
    }

    // Camera
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanline remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color{0.0, 0.0, 0.0};
            for (int k = 0; k < samples_per_pixel; ++k) {
                // ray
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                ray ray = cam.get_ray(u, v);
                pixel_color += ray_color(ray, world, depth);

            }
            write_color(pixel_color, samples_per_pixel, std::cout);
        }
    }

    std::cerr << "\rDone.\n";

    return 0;
}