#ifndef SHAPES_DISK_H
#define SHAPES_DISK_H

// shapes/disk.h*
#include "shape.h"

namespace pbrt {

// Disk Declarations
class Disk : public Shape {
  public:
    // Disk Public Methods
    Disk(const Transform *ObjectToWorld, const Transform *WorldToObject,
         bool reverseOrientation, float height, float radius, float innerRadius,
         float phiMax)
        : Shape(ObjectToWorld, WorldToObject, reverseOrientation),
          height(height),
          radius(radius),
          innerRadius(innerRadius),
          phiMax(Radians(Clamp(phiMax, 0, 360))) {}
    Bounds3f ObjectBound() const;
    bool Intersect(const Ray &ray, float *tHit, SurfaceInteraction *isect,
                   bool testAlphaTexture) const;
    bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
    float Area() const;
    Interaction Sample(const Point2f &u, float *pdf) const;

  private:
    // Disk Private Data
    const float height, radius, innerRadius, phiMax;
};

std::shared_ptr<Disk> CreateDiskShape(const Transform *o2w,
                                      const Transform *w2o,
                                      bool reverseOrientation,
                                      const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SHAPES_DISK_H
