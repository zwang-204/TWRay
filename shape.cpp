// core/shape.cpp*
#include "shape.h"
#include "stats.h"
#include "lowdiscrepancy.h"

namespace pbrt {

// Shape Method Definitions
Shape::~Shape() {}

STAT_COUNTER("Scene/Shapes created", nShapesCreated);
Shape::Shape(const Transform *ObjectToWorld, const Transform *WorldToObject,
             bool reverseOrientation)
    : ObjectToWorld(ObjectToWorld),
      WorldToObject(WorldToObject),
      reverseOrientation(reverseOrientation),
      transformSwapsHandedness(ObjectToWorld->SwapsHandedness()) {
    ++nShapesCreated;
}

Bounds3f Shape::WorldBound() const { return (*ObjectToWorld)(ObjectBound()); }

Interaction Shape::Sample(const Interaction &ref, const Point2f &u,
                          float *pdf) const {
    Interaction intr = Sample(u, pdf);
    Vector3f wi = intr.p - ref.p;
    if (wi.LengthSquared() == 0)
        *pdf = 0;
    else {
        wi = Normalize(wi);
        // Convert from area measure, as returned by the Sample() call
        // above, to solid angle measure.
        *pdf *= DistanceSquared(ref.p, intr.p) / AbsDot(intr.n, -wi);
        if (std::isinf(*pdf)) *pdf = 0.f;
    }
    return intr;
}

float Shape::Pdf(const Interaction &ref, const Vector3f &wi) const {
    // Intersect sample ray with area light geometry
    Ray ray = ref.SpawnRay(wi);
    float tHit;
    SurfaceInteraction isectLight;
    // Ignore any alpha textures used for trimming the shape when performing
    // this intersection. Hack for the "San Miguel" scene, where this is used
    // to make an invisible area light.
    if (!Intersect(ray, &tHit, &isectLight, false)) return 0;

    // Convert light sample weight to solid angle measure
    float pdf = DistanceSquared(ref.p, isectLight.p) /
                (AbsDot(isectLight.n, -wi) * Area());
    if (std::isinf(pdf)) pdf = 0.f;
    return pdf;
}

float Shape::SolidAngle(const Point3f &p, int nSamples) const {
    Interaction ref(p, Normal3f(), Vector3f(), Vector3f(0, 0, 1), 0,
                    MediumInterface{});
    double solidAngle = 0;
    for (int i = 0; i < nSamples; ++i) {
        Point2f u{RadicalInverse(0, i), RadicalInverse(1, i)};
        float pdf;
        Interaction pShape = Sample(ref, u, &pdf);
        if (pdf > 0 && !IntersectP(Ray(p, pShape.p - p, .999f))) {
            solidAngle += 1 / pdf;
        }
    }
    return solidAngle / nSamples;
}

}  // namespace pbrt
