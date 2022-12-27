#ifndef PBRT_FILTERS_BOX_H
#define PBRT_FILTERS_BOX_H

// filters/box.h*
#include "filter.h"

namespace pbrt {

// Box Filter Declarations
class BoxFilter : public Filter {
  public:
    BoxFilter(const Vector2f &radius) : Filter(radius) {}
    float Evaluate(const Point2f &p) const;
};

BoxFilter *CreateBoxFilter(const ParamSet &ps);

}  // namespace pbrt

#endif  // PBRT_FILTERS_BOX_H