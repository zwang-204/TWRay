// integrators/directlighting.cpp*
#include "directlighting.h"
#include "../interaction.h"
#include "../paramset.h"
#include "../camera.h"
#include "../film.h"
#include "../stats.h"

namespace pbrt {

// DirectLightingIntegrator Method Definitions
void DirectLightingIntegrator::Preprocess(const Scene &scene,
                                          Sampler &sampler) {
    if (strategy == LightStrategy::UniformSampleAll) {
        // Compute number of samples to use for each light
        for (const auto &light : scene.lights)
            nLightSamples.push_back(sampler.RoundCount(light->nSamples));

        // Request samples for sampling all lights
        for (int i = 0; i < maxDepth; ++i) {
            for (size_t j = 0; j < scene.lights.size(); ++j) {
                sampler.Request2DArray(nLightSamples[j]);
                sampler.Request2DArray(nLightSamples[j]);
            }
        }
    }
}

Spectrum DirectLightingIntegrator::Li(const RayDifferential &ray,
                                      const Scene &scene, Sampler &sampler,
                                      MemoryArena &arena, int depth) const {
    ProfilePhase p(Prof::SamplerIntegratorLi);
    Spectrum L(0.f);
    // Find closest ray intersection or return background radiance
    SurfaceInteraction isect;
    if (!scene.Intersect(ray, &isect)) {
        for (const auto &light : scene.lights) L += light->Le(ray);
        return L;
    }

    // Compute scattering functions for surface interaction
    isect.ComputeScatteringFunctions(ray, arena);
    if (!isect.bsdf)
        return Li(isect.SpawnRay(ray.d), scene, sampler, arena, depth);
    Vector3f wo = isect.wo;
    // Compute emitted light if ray hit an area light source
    L += isect.Le(wo);
    if (scene.lights.size() > 0) {
        // Compute direct lighting for _DirectLightingIntegrator_ integrator
        if (strategy == LightStrategy::UniformSampleAll)
            L += UniformSampleAllLights(isect, scene, arena, sampler,
                                        nLightSamples);
        else
            L += UniformSampleOneLight(isect, scene, arena, sampler);
    }
    if (depth + 1 < maxDepth) {
        // Trace rays for specular reflection and refraction
        L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
        L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
    }
    return L;
}

DirectLightingIntegrator *CreateDirectLightingIntegrator(
    const ParamSet &params, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<const Camera> camera) {
    int maxDepth = params.FindOneInt("maxdepth", 5);
    LightStrategy strategy;
    std::string st = params.FindOneString("strategy", "all");
    if (st == "one")
        strategy = LightStrategy::UniformSampleOne;
    else if (st == "all")
        strategy = LightStrategy::UniformSampleAll;
    else {
        Warning(
            "Strategy \"%s\" for direct lighting unknown. "
            "Using \"all\".",
            st.c_str());
        strategy = LightStrategy::UniformSampleAll;
    }
    int np;
    const int *pb = params.FindInt("pixelbounds", &np);
    Bounds2i pixelBounds = camera->film->GetSampleBounds();
    if (pb) {
        if (np != 4)
            Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
                  np);
        else {
            pixelBounds = Intersect(pixelBounds,
                                    Bounds2i{{pb[0], pb[2]}, {pb[1], pb[3]}});
            if (pixelBounds.Area() == 0)
                Error("Degenerate \"pixelbounds\" specified.");
        }
    }
    return new DirectLightingIntegrator(strategy, maxDepth, camera, sampler,
                                        pixelBounds);
}

}  // namespace pbrt
