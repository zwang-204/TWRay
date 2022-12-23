#ifndef INTEGRATORS_DIRECTLIGHTING_H
#define INTEGRATORS_DIRECTLIGHTING_H

// integrators/directlighting.h*
#include "../pbrt.h"
#include "../integrator.h"
#include "../scene.h"

namespace pbrt {

// LightStrategy Declarations
enum class LightStrategy { UniformSampleAll, UniformSampleOne };

// DirectLightingIntegrator Declarations
class DirectLightingIntegrator : public SamplerIntegrator {
  public:
    // DirectLightingIntegrator Public Methods
    DirectLightingIntegrator(LightStrategy strategy, int maxDepth,
                             std::shared_ptr<const Camera> camera,
                             std::shared_ptr<Sampler> sampler,
                             const Bounds2i &pixelBounds)
        : SamplerIntegrator(camera, sampler, pixelBounds),
          strategy(strategy),
          maxDepth(maxDepth) {}
    Spectrum Li(const RayDifferential &ray, const Scene &scene,
                Sampler &sampler, MemoryArena &arena, int depth) const;
    void Preprocess(const Scene &scene, Sampler &sampler);

  private:
    // DirectLightingIntegrator Private Data
    const LightStrategy strategy;
    const int maxDepth;
    std::vector<int> nLightSamples;
};

DirectLightingIntegrator *CreateDirectLightingIntegrator(
    const ParamSet &params, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<const Camera> camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_DIRECTLIGHTING_H
