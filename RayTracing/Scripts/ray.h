#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    public:
        ray() {}
        ray(const Point3f& origin, const Vector3f& direction, float time = 0.0)
            : orig(origin), dir(direction), tm(time)
        {}

        Point3f origin() const { return orig; }
        Vector3f direction() const { return dir; }
        float time() const { return tm; }
        
        Point3f at(float t) const {
            return orig + t*dir;
        }
    
    public:
        Point3f orig;
        Vector3f dir;
        float tm;
};

#endif