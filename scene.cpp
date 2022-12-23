// core/scene.cpp*
#include "scene.h"
#include "stats.h"

namespace pbrt {

STAT_COUNTER("Intersections/Regular ray intersection tests",
             nIntersectionTests);
STAT_COUNTER("Intersections/Shadow ray intersection tests", nShadowTests);

// Scene Method Definitions
bool Scene::Intersect(const Ray &ray, SurfaceInteraction *isect) const {
    ++nIntersectionTests;
    DCHECK_NE(ray.d, Vector3f(0,0,0));
    return aggregate->Intersect(ray, isect);
}

bool Scene::IntersectP(const Ray &ray) const {
    ++nShadowTests;
    DCHECK_NE(ray.d, Vector3f(0,0,0));
    return aggregate->IntersectP(ray);
}

bool Scene::IntersectTr(Ray ray, Sampler &sampler, SurfaceInteraction *isect,
                        Spectrum *Tr) const {
    *Tr = Spectrum(1.f);
    while (true) {
        bool hitSurface = Intersect(ray, isect);
        // Accumulate beam transmittance for ray segment
        if (ray.medium) *Tr *= ray.medium->Tr(ray, sampler);

        // Initialize next ray segment or terminate transmittance computation
        if (!hitSurface) return false;
        if (isect->primitive->GetMaterial() != nullptr) return true;
        ray = isect->SpawnRay(ray.d);
    }
}

}  // namespace pbrt
