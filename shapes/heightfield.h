#ifndef SHAPES_HEIGHTFIELD_H
#define SHAPES_HEIGHTFIELD_H

// shapes/heightfield.h*
#include "../shape.h"

namespace pbrt {

// Heightfield Declarations
std::vector<std::shared_ptr<Shape>> CreateHeightfield(const Transform *o2w,
                                                      const Transform *w2o,
                                                      bool ro,
                                                      const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SHAPES_HEIGHTFIELD_H