#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "rtweekend.h"
#include "hittable.h"
#include "aabb.h"

/*
class moving_sphere : public hittable{
    public:
        moving_sphere() {}
        moving_sphere(
            Point3f cen0, Point3f cen1, double _time0, double _time1, double r, shared_ptr<material> m)
            : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
        {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(
            double _time0, double _time1, aabb& output_box) const override;
        

        Point3f center(double time) const;
    
    public:
        Point3f center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};

Point3f moving_sphere::center(double time) const {
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    Vector3f oc = r.origin() - center(r.time());
    auto a = r.direction().LengthSquared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.LengthSquared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root){
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vector3f outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const {
    aabb box0(
        center(_time0) - Vector3f(radius, radius, radius),
        center(_time0) + Vector3f(radius, radius, radius));
    aabb box1(
        center(_time1) - Vector3f(radius, radius, radius),
        center(_time1) + Vector3f(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}
*/

#endif