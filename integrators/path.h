#ifndef INTEGRATORS_PATH_H
#define INTEGRATORS_PATH_H

// integrators/path.h*
#include "../pbrt.h"
#include "../integrator.h"
#include "../lightdistrib.h"

namespace pbrt {

// PathIntegrator Declarations
class PathIntegrator : public SamplerIntegrator {
  public:
    // PathIntegrator Public Methods
    PathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
                   std::shared_ptr<Sampler> sampler,
                   const Bounds2i &pixelBounds, float rrThreshold = 1,
                   const std::string &lightSampleStrategy = "spatial");

    void Preprocess(const Scene &scene, Sampler &sampler);
    Spectrum Li(const RayDifferential &ray, const Scene &scene,
                Sampler &sampler, MemoryArena &arena, int depth) const;

  private:
    // PathIntegrator Private Data
    const int maxDepth;
    const float rrThreshold;
    const std::string lightSampleStrategy;
    std::unique_ptr<LightDistribution> lightDistribution;
};

PathIntegrator *CreatePathIntegrator(const ParamSet &params,
                                     std::shared_ptr<Sampler> sampler,
                                     std::shared_ptr<const Camera> camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_PATH_H
