#ifndef CYLINDER_H
#define CYLINDER_H

#include "hittable.h"
#include "vec3.h"

class cylinder : public hittable {
    public:
        cylinder() {}
        cylinder(double r, vec3 c, double zMin, double zMax, double phiMax, shared_ptr<material> m) 
            : radius(r), center(c), zMin(zMin), zMax(zMax), phiMax(phiMax), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        virtual double pdf_value(const point3& o, const vec3& v) const override;
        virtual vec3 random(const point3& o) const override;
    
    public:
        double radius;
        vec3 center;
        double zMin;
        double zMax;
        double phiMax;
        shared_ptr<material> mat_ptr;
    
    private:
        static void get_cylinder_uv(const point3& p, const double& phi, const double& zMin, const double& zMax, const double& phiMax,
            double& u, double& v) {
            u = phi / phiMax;
            v = (p.z() - zMin) / (zMax - zMin);
        }
};

bool cylinder::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    vec3 dir = r.direction();
    auto a = dir.x()*dir.x() + dir.y()*dir.y();
    auto half_b = dir.x()*oc.x()+dir.y()*oc.y();
    auto c = oc.x()*oc.x() + oc.y()*oc.y() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root){
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    vec3 pHit = oc + dir * root;
    double phi = atan2(pHit.y(), pHit.x());
    if (phi < 0) phi += 2 * pi;

    if (pHit.z() < zMin || pHit.z() > zMax || phi > phiMax) {
        return false;
    }


    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal;
    if(pHit.z() == zMin){
        outward_normal = vec3(0,0,-1);
    }
    else if(pHit.z() == zMax){
        outward_normal = vec3(0,0,1);
    }
    else{
        outward_normal = vec3(pHit.x(), pHit.y(), 0)/radius;
    }
    rec.set_face_normal(r, outward_normal);
    get_cylinder_uv(pHit, phi, zMin, zMax, phiMax, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool cylinder::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        vec3(-radius, -radius, zMin),
        vec3(radius, radius, zMax));
    return true;
}

double cylinder::pdf_value(const point3& o, const vec3& v) const {
    hit_record rec;
    if(!this->hit(ray(o, v), 0.001, infinity, rec))
        return 0;
    
    auto cos_theta_max = sqrt(1 - radius*radius/(center-o).length_squared());
    auto solid_angle = 2*pi*(1-cos_theta_max);

    return 1 / solid_angle;
}

vec3 cylinder::random(const point3& o) const {
    vec3 direction = center - o;
    auto distance_squared = direction.length_squared();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
}

#endif