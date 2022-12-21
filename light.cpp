// core/light.cpp*
#include "light.h"
//#include "scene.h"
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

}  // namespace pbrt
