#ifndef INTERACTION_H
#define INTERACTION_H

#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "shape.h"

namespace pbrt{

// Interaction Declarations
struct Interaction {
    // Interaction Public Methods
    Interaction() : time(0) {}
    Interaction(const Point3f &p, const Normal3f &n, const Vector3f &pError,
                const Vector3f &wo, float time)
        : p(p),
          time(time),
          pError(pError),
          wo(Normalize(wo)),
          n(n) {}
    bool IsSurfaceInteraction() const { return n != Normal3f(); }
    Interaction(const Point3f &p, const Vector3f &wo, float time)
        : p(p), time(time), wo(wo) {}
    Interaction(const Point3f &p, float time)
        : p(p), time(time) {}
    bool IsMediumInteraction() const { return !IsSurfaceInteraction(); }

    // Interaction Public Data
    Point3f p;
    float time;
    Vector3f pError;
    Vector3f wo;
    Normal3f n;
};

// SurfaceInteraction Declarations
class SurfaceInteraction : public Interaction {
  public:
    // SurfaceInteraction Public Methods
    SurfaceInteraction() {}
    SurfaceInteraction(const Point3f &p, const Vector3f &pError,
                       const Point2f &uv, const Vector3f &wo,
                       const Vector3f &dpdu, const Vector3f &dpdv,
                       const Normal3f &dndu, const Normal3f &dndv, float time,
                       const Shape *sh,
                       int faceIndex = 0);
    void SetShadingGeometry(const Vector3f &dpdu, const Vector3f &dpdv,
                            const Normal3f &dndu, const Normal3f &dndv,
                            bool orientationIsAuthoritative);
    void ComputeScatteringFunctions(
        const RayDifferential &ray, 
        bool allowMultipleLobes = false);
    void ComputeDifferentials(const RayDifferential &r) const;

    // SurfaceInteraction Public Data
    Point2f uv;
    Vector3f dpdu, dpdv;
    Normal3f dndu, dndv;
    const Shape *shape = nullptr;
    struct {
        Normal3f n;
        Vector3f dpdu, dpdv;
        Normal3f dndu, dndv;
    } shading;
    mutable Vector3f dpdx, dpdy;
    mutable float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

    // Added after book publication. Shapes can optionally provide a face
    // index with an intersection point for use in Ptex texture lookups.
    // If Ptex isn't being used, then this value is ignored.
    int faceIndex = 0;
};

}

#endif