#ifndef FILTER_H
#define FILTER_H

// core/filter.h*
#include "pbrt.h"
#include "geometry.h"
#include "paramset.h"

namespace pbrt {

// Filter Declarations
class Filter {
  public:
    // Filter Interface
    virtual ~Filter();
    Filter(const Vector2f &radius)
        : radius(radius), invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}
    virtual float Evaluate(const Point2f &p) const = 0;

    // Filter Public Data
    const Vector2f radius, invRadius;
};

}  // namespace pbrt

#endif  // PBRT_CORE_FILTER_H