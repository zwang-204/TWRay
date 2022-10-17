#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(Point3f cen, float r, shared_ptr<material> m) 
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, float t_min, float t_max, hit_record& rec) const override;
        virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;
        virtual float pdf_value(const Point3f& o, const Vector3f& v) const override;
        virtual Vector3f random(const Point3f& o) const override;
    
    public:
        Point3f center;
        float radius;
        shared_ptr<material> mat_ptr;
    
    private:
        static void get_sphere_uv(const Vector3f& p, float& u, float& v) {
            auto theta = acos(-p.y);
            auto phi = atan2(-p.z, p.x) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    Vector3f oc = r.origin() - center;
    auto a = r.direction().LengthSquared();
    auto half_b = Dot(oc, r.direction());
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
    Vector3f outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
    output_box = aabb(
        center - Vector3f(radius, radius, radius),
        center + Vector3f(radius, radius, radius));
    return true;
}

float sphere::pdf_value(const Point3f& o, const Vector3f& v) const {
    hit_record rec;
    if(!this->hit(ray(o, v), 0.001, infinity, rec))
        return 0;
    
    auto cos_theta_max = sqrt(1 - radius*radius/(center-o).LengthSquared());
    auto solid_angle = 2*pi*(1-cos_theta_max);

    return 1 / solid_angle;
}

Vector3f sphere::random(const Point3f& o) const {
    Vector3f direction = center - o;
    auto distance_squared = direction.LengthSquared();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
}

#endif