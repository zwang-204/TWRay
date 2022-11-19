#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

namespace pbrt{

class hittable_list : public hittable {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const Ray& r, float t_min, float t_max, SurfaceInteraction& si) const override;
        virtual bool bounding_box(
            Bounds3f& output_box) const override;
        virtual float pdf_value(const Point3f& o, const Vector3f& v) const override;
        virtual Vector3f random(const Point3f& o) const override;
    
    public: 
        std::vector<shared_ptr<hittable>> objects;

};

bool hittable_list::hit(const Ray& r, float t_min, float t_max, SurfaceInteraction& si) const{
    SurfaceInteraction temp_si;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    //Bounds3f bounding_boxes = 

    for (const auto& object : objects){
        if (object->hit(r, t_min, closest_so_far, temp_si)) {
            hit_anything = true;
            closest_so_far = temp_si.time;
            si =  temp_si;
        }
    }

    return hit_anything;
}

bool hittable_list::bounding_box(Bounds3f& output_box) const {
    if (objects.empty()) return false;

    Bounds3f temp_box; 
    bool first_box = true;

    

    for(const auto& object : objects){
        if(!object->bounding_box(temp_box)) return false;
        //why Bounds3f = bool
        output_box = first_box ? temp_box : Union(output_box, temp_box);
        first_box = false;
    }

    return true;
}

float hittable_list::pdf_value(const Point3f& o, const Vector3f& v) const {
    auto weight = 1.0/objects.size();
    auto sum = 0.0;

    for (const auto& object : objects)
        sum += weight * object->pdf_value(o, v);
    
    return sum;
}

Vector3f hittable_list::random(const Point3f& o) const {
    auto int_size = static_cast<int>(objects.size());
    return objects[random_int(0, int_size-1)]->random(o);
}

}

#endif