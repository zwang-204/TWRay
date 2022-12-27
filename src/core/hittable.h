#ifndef HITTABLE_H
#define HITTABLE_H

#include "pbrt.h"
#include "geometry.h"

namespace pbrt{


class Material;

class hittable {
    public:
        virtual bool hit(const Ray& r, float t_min, float t_max, Interaction& si) const = 0;
        virtual bool bounding_box(Bounds3f& output_box) const = 0;
        virtual float pdf_value(const Point3f& o, const Vector3f& v) const {
            return 0.0;
        }

        virtual Vector3f random(const Point3f& o) const {
            return Vector3f(1, 0, 0);
        }
};

// class translate: public hittable {
//     public:
//         translate(shared_ptr<hittable> p, const Vector3f& displacement)
//             : ptr(p), offset(displacement) {}

//         virtual bool hit(
//             const Ray& r, float t_min, float t_max, Interaction& si) const override;
        
//         virtual bool bounding_box(Bounds3f& output_box) const override;

//     public:
//         shared_ptr<hittable> ptr;
//         Vector3f offset;
// };

// bool translate::hit(const Ray& r, float t_min, float t_max, Interaction& si) const {
//     Ray moved_r(r.origin() - offset, r.direction());
//     if (!ptr->hit(moved_r, t_min, t_max, si))
//         return false;
//     si.p += offset;
//     //si.set_face_normal(moved_r, si.n);

//     return true;
// }

// bool translate::bounding_box(Bounds3f& output_box) const {
//     // infinite recursion?
//     if(!ptr->bounding_box(output_box))
//         return false;
    
//     output_box = Bounds3f(
//         output_box.Min() + offset,
//         output_box.Max() + offset);
    
//     return true;
// }

// class rotate_y : public hittable {
//     public: 
//         rotate_y(shared_ptr<hittable> p, float angle);

//         virtual bool hit(
//             const Ray& r, float t_min, float t_max, Interaction& si) const override;

//         virtual bool bounding_box(Bounds3f& output_box) const override {
//             output_box = bbox;
//             return hasbox;
//         }
//     public:
//         shared_ptr<hittable> ptr;
//         float sin_theta;
//         float cos_theta;
//         bool hasbox;
//         Bounds3f bbox;
// };

// rotate_y::rotate_y(shared_ptr<hittable> p, float angle) : ptr(p) {
//     auto radians = Radians(angle);
//     sin_theta = sin(radians);
//     cos_theta = cos(radians);
//     hasbox = ptr->bounding_box(bbox);

//     Point3f min(Infinity, Infinity, Infinity);
//     Point3f max(-Infinity, -Infinity, -Infinity);
    
//     for(int i = 0; i < 2; i++){
//         for(int j = 0; j < 2; j++){
//             for(int k = 0; k < 2; k++){
//                 auto x = i*bbox.Max().x + (1-i)*bbox.Min().x;
//                 auto y = j*bbox.Max().y + (1-j)*bbox.Min().y;
//                 auto z = k*bbox.Max().z + (1-k)*bbox.Min().z;

//                 auto newx = cos_theta*x + sin_theta*z;
//                 auto newz = -sin_theta*x + cos_theta*z;

//                 Vector3f tester(newx, y, newz);
                
//                 for(int c = 0; c < 3; c++){
//                     min[c] = fminf(min[c], tester[c]);
//                     max[c] = fmaxf(max[c], tester[c]);
//                 }
//             }
//         }

//         bbox = Bounds3f(min, max);
//     }
// }

// bool rotate_y::hit(const Ray& r, float t_min, float t_max, Interaction& si) const {
//     auto origin = r.origin();
//     auto direction = r.direction();

//     origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2]; 
//     origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

//     direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2]; 
//     direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

//     Ray rotated_r(origin, direction);

//     if(!ptr->hit(rotated_r, t_min, t_max, si))
//         return false;
    
//     auto p = si.p;
//     auto normal = si.n;

//     p[0] = cos_theta*si.p[0] + sin_theta*si.p[2];
//     p[2] = -sin_theta*si.p[0] + cos_theta*si.p[2];

//     normal[0] = cos_theta*si.n[0] + sin_theta*si.n[2];
//     normal[2] = -sin_theta*si.n[0] + cos_theta*si.n[2];

//     si.p = p;
//     si.set_face_normal(rotated_r, normal);

//     return true;
// }

// class flip_face : public hittable {
//     public:
//         flip_face(shared_ptr<hittable> p) : ptr(p) {}

//         virtual bool hit(
//             const Ray& r, float t_min, float t_max, Interaction& si) const override{
            
//             if (!ptr->hit(r, t_min, t_max, si))
//                 return false;
            
//             si.front_face = !si.front_face;
//             return true;

//         }

//         virtual bool bounding_box(Bounds3f& output_box) const override {
//             return ptr->bounding_box(output_box);
//         }
    
//     public:
//         shared_ptr<hittable> ptr;
// };

}

#endif