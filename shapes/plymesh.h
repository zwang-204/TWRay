#ifndef SHAPES_PLYMESH_H
#define SHAPES_PLYMESH_H

// shapes/plymesh.h*
#include "triangle.h"

namespace pbrt {

std::vector<std::shared_ptr<Shape>> CreatePLYMesh(
    const Transform *o2w, const Transform *w2o, bool reverseOrientation,
    const ParamSet &params,
    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures =
        nullptr);

}  // namespace pbrt

#endif  // PBRT_SHAPES_PLYMESH_H