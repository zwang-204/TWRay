#ifndef INTEGRATORS_SPPM_H
#define INTEGRATORS_SPPM_H

// integrators/sppm.h*
#include "pbrt.h"
#include "integrator.h"
#include "camera.h"
#include "film.h"

namespace pbrt {

// SPPM Declarations
class SPPMIntegrator : public Integrator {
  public:
    // SPPMIntegrator Public Methods
    SPPMIntegrator(std::shared_ptr<const Camera> &camera, int nIterations,
                   int photonsPerIteration, int maxDepth,
                   float initialSearchRadius, int writeFrequency)
        : camera(camera),
          initialSearchRadius(initialSearchRadius),
          nIterations(nIterations),
          maxDepth(maxDepth),
          photonsPerIteration(photonsPerIteration > 0
                                  ? photonsPerIteration
                                  : camera->film->croppedPixelBounds.Area()),
          writeFrequency(writeFrequency) {}
    void Render(const Scene &scene);

  private:
    // SPPMIntegrator Private Data
    std::shared_ptr<const Camera> camera;
    const float initialSearchRadius;
    const int nIterations;
    const int maxDepth;
    const int photonsPerIteration;
    const int writeFrequency;
};

Integrator *CreateSPPMIntegrator(const ParamSet &params,
                                 std::shared_ptr<const Camera> camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_SPPM_H
