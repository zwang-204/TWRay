#include "primitive.h"
//#include "light.h"
#include "memory.h"
#include "interaction.h"
#include "stats.h"

namespace pbrt {

STAT_MEMORY_COUNTER("Memory/Primitives", primitiveMemory);

// Primitive Method Definitions
Primitive::~Primitive() {}

const Material *Aggregate::GetMaterial() const {
    std::cerr <<
        "Aggregate::GetMaterial() method"
        "called; should have gone to GeometricPrimitive";
    return nullptr;
}

void Aggregate::ComputeScatteringFunctions(SurfaceInteraction *isect,
                                           MemoryArena &arena,
                                           TransportMode mode,
                                           bool allowMultipleLobes) const {
    std::cerr <<
        "Aggregate::ComputeScatteringFunctions() method"
        "called; should have gone to GeometricPrimitive";
}

bool TransformedPrimitive::Intersect(const Ray &r,
                                     SurfaceInteraction *isect) const {
    // Compute _ray_ after transformation by _PrimitiveToWorld_
    Transform InterpolatedPrimToWorld;
    PrimitiveToWorld.Interpolate(r.time, &InterpolatedPrimToWorld);
    Ray ray = Inverse(InterpolatedPrimToWorld)(r);
    if (!primitive->Intersect(ray, isect)) return false;
    r.tMax = ray.tMax;
    // Transform instance's intersection data to world space
    // if (!InterpolatedPrimToWorld.IsIdentity())
    //    *isect = InterpolatedPrimToWorld(*isect);
    CHECK_GE(Dot(isect->n, isect->shading.n), 0);
    return true;
}

bool TransformedPrimitive::IntersectP(const Ray &r) const {
    Transform InterpolatedPrimToWorld;
    PrimitiveToWorld.Interpolate(r.time, &InterpolatedPrimToWorld);
    Transform InterpolatedWorldToPrim = Inverse(InterpolatedPrimToWorld);
    return primitive->IntersectP(InterpolatedWorldToPrim(r));
}

// GeometricPrimitive Method Definitions
GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape> &shape,
                                       const std::shared_ptr<Material> &material)
    : shape(shape),
    material(material){
    primitiveMemory += sizeof(*this);
}

Bounds3f GeometricPrimitive::WorldBound() const { return shape->WorldBound(); }

bool GeometricPrimitive::IntersectP(const Ray &r) const {
    return shape->IntersectP(r);
}

bool GeometricPrimitive::Intersect(const Ray &r,
                                   SurfaceInteraction *isect) const {
    float tHit;
    if (!shape->Intersect(r, &tHit, isect)) return false;
    r.tMax = tHit;
    isect->primitive = this;
    CHECK_GE(Dot(isect->n, isect->shading.n), 0.);
    return true;
}

const Material *GeometricPrimitive::GetMaterial() const {
    return material.get();
}

void GeometricPrimitive::ComputeScatteringFunctions(
    SurfaceInteraction *isect, MemoryArena &arena, TransportMode mode,
    bool allowMultipleLobes) const {
    // if (material)
    //     material->ComputeScatteringFunctions(isect, arena, mode,
    //                                          allowMultipleLobes);
    CHECK_GE(Dot(isect->n, isect->shading.n), 0.);
}

}  // namespace pbrt