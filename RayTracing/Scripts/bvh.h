#ifndef BVH_H
#define BVH_H

#include "pbrt.h"

#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

namespace pbrt{

class bvh_node : public hittable  {
    public:
        bvh_node();

        bvh_node(const hittable_list& list)
            : bvh_node(list.objects, 0, list.objects.size())
        {}

        bvh_node(
            const std::vector<shared_ptr<hittable>>& src_objects,
            size_t start, size_t end);

        virtual bool hit(
            const Ray& r, float t_min, float t_max, hit_record& rec) const override;

        virtual bool bounding_box(Bounds3f& output_box) const override;

    public:
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        Bounds3f box;
};


inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    Bounds3f box_a;
    Bounds3f box_b;

    if (!a->bounding_box(box_a) || !b->bounding_box(box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.Min()[axis] < box_b.Max()[axis];
}


bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}


bvh_node::bvh_node(
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end
) {
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = random_int(0,2);
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start + 1;

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        } else {
            left = objects[start+1];
            right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end - 1, comparator);

        auto mid = start + object_span/2;
        left = make_shared<bvh_node>(objects, start, mid);
        right = make_shared<bvh_node>(objects, mid, end);
    }
    
    Bounds3f box_left, box_right;

    if (  !left->bounding_box(box_left)
       || !right->bounding_box(box_right)
    )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = Union(box_left, box_right);
}


bool bvh_node::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {

    if (!box.IntersectP(r, &t_min, &t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, t_max, rec);

    return hit_left || hit_right;
}


bool bvh_node::bounding_box(Bounds3f& output_box) const {
    output_box = box;
    return true;
}

}

#endif