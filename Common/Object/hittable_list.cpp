#include "hittable_list.h"

hittable_list::hittable_list(const std::shared_ptr<hittable> &object) { add(object); }

void hittable_list::clear() { objects.clear(); }
void hittable_list::add(const std::shared_ptr<hittable> &object) { objects.push_back(object); }

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    double tmp_t_max = t_max;
    bool hit = false;
    for (const auto &object : objects) {
        if (object->hit(r, t_min, tmp_t_max, rec)) {
            tmp_t_max = rec.t; // 更新碰撞点的最大时间
            hit = true;
        }
    }

    return hit;
}

bool hittable_list::bounding_box(double time0, double time1, aabb &output_box) const {
    if (objects.empty())
        return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto &object : objects) {
        // 求出单个物体的包围盒
        if (!object->bounding_box(time0, time1, temp_box)) return false;

        // 如果不是第一个物体，求该物体与之前物体的包围盒并集
        output_box = first_box ? temp_box : aabb::surrounding_box(output_box, temp_box);
        first_box = false;
    }
    return true;
}
