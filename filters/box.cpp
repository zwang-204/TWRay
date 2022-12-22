#include "box.h"
#include "../paramset.h"

namespace pbrt {

// Box Filter Method Definitions
float BoxFilter::Evaluate(const Point2f &p) const { return 1.; }

BoxFilter *CreateBoxFilter(const ParamSet &ps) {
    float xw = ps.FindOneFloat("xwidth", 0.5f);
    float yw = ps.FindOneFloat("ywidth", 0.5f);
    return new BoxFilter(Vector2f(xw, yw));
}

}  // namespace pbrt
