// core/light.cpp*
#include "light.h"
#include "scene.h"
#include "sampling.h"
#include "stats.h"
#include "paramset.h"

namespace pbrt {

//STAT_COUNTER("Scene/Lights", numLights);
//STAT_COUNTER("Scene/AreaLights", numAreaLights);

// Light Method Definitions
Light::Light(int flags, const Transform &LightToWorld,  int nSamples)
    : flags(flags),
      nSamples(std::max(1, nSamples)),
      LightToWorld(LightToWorld),
      WorldToLight(Inverse(LightToWorld)) {
    //++numLights;
}

Light::~Light() {}

Spectrum Light::Le(const RayDifferential &ray) const { return Spectrum(0.f); }

AreaLight::AreaLight(const Transform &LightToWorld,
                     int nSamples)
    : Light((int)LightFlags::Area, LightToWorld, nSamples) {
    //++numAreaLights;
}

bool VisibilityTester::Unoccluded(const Scene &scene) const {
    return !scene.IntersectP(p0.SpawnRayTo(p1));
}

Spectrum VisibilityTester::Tr(const Scene &scene, Sampler &sampler) const {
    Ray ray(p0.SpawnRayTo(p1));
    Spectrum Tr(1.f);
    while (true) {
        SurfaceInteraction isect;
        bool hitSurface = scene.Intersect(ray, &isect);
        // Handle opaque surface along ray's path
        if (hitSurface && isect.primitive->GetMaterial() != nullptr)
            return Spectrum(0.0f);

        // Update transmittance for current ray segment
        // if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

        // Generate next ray segment or return final transmittance
        if (!hitSurface) break;
        ray = isect.SpawnRayTo(p1);
    }
    return Tr;
}

}  // namespace pbrt
