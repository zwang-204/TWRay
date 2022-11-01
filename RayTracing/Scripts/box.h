#ifndef BOX_H
#define BOX_H

#include "pbrt.h"

#include "aarect.h"
#include "hittable_list.h"
#include "geometry.h"

namespace pbrt{

class box : public hittable {
    public:
        box() {}
        box(const Point3f& p0, const Point3f& p1, shared_ptr<material> ptr);

        virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override;

        virtual bool bounding_box(Bounds3f& output_box) const override {
            output_box = Bounds3f(box_min, box_max);
            return true;
        }
    
    public:
        Point3f box_min;
        Point3f box_max;
        hittable_list sides;
};

box::box(const Point3f& p0, const Point3f& p1, shared_ptr<material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p1.z, ptr));
    sides.add(make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));

    sides.add(make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p1.y, ptr));
    sides.add(make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));

    sides.add(make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p1.x, ptr));
    sides.add(make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
}

bool box::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}

}

#endif