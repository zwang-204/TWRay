#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
    public:
        aabb() {}
        aabb(const Point3f& a, const Point3f& b) { minimum = a; maximum = b;}

        Point3f min() const {return minimum; }
        Point3f max() const {return maximum; }

        bool hit(const ray& r, float t_min, float t_max) const {
            for(int a = 0; a < 3; a++) {
                auto t0 = fminf((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                auto t1 = fmaxf((minimum[a] - r.origin()[a]) / r.direction()[a],
                               (maximum[a] - r.origin()[a]) / r.direction()[a]);
                t_min = fmax(t0, t_min);
                t_max = fmin(t1, t_max);
                if(t_max <= t_min)
                    return false;
            }
            return true;
        }

        Point3f minimum;
        Point3f maximum;
};

aabb surrounding_box(aabb box0, aabb box1) {
    Point3f small(fmin(box0.min().x, box1.min().x),
                 fmin(box0.min().y, box1.min().y),
                 fmin(box0.min().z, box1.min().z));
    Point3f big(fmax(box0.max().x, box1.max().x),
                 fmax(box0.max().y, box1.max().y),
                 fmax(box0.max().z, box1.max().z));
    
    return aabb(small, big);
}

#endif