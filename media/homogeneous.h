#ifndef MEDIA_HOMOGENEOUS_H
#define MEDIA_HOMOGENEOUS_H

// media/homogeneous.h*
#include "../medium.h"

namespace pbrt {

// HomogeneousMedium Declarations
class HomogeneousMedium : public Medium {
  public:
    // HomogeneousMedium Public Methods
    HomogeneousMedium(const Spectrum &sigma_a, const Spectrum &sigma_s, float g)
        : sigma_a(sigma_a),
          sigma_s(sigma_s),
          sigma_t(sigma_s + sigma_a),
          g(g) {}
    Spectrum Tr(const Ray &ray, Sampler &sampler) const;
    Spectrum Sample(const Ray &ray, Sampler &sampler, MemoryArena &arena,
                    MediumInteraction *mi) const;

  private:
    // HomogeneousMedium Private Data
    const Spectrum sigma_a, sigma_s, sigma_t;
    const float g;
};

}  // namespace pbrt

#endif  // PBRT_MEDIA_HOMOGENEOUS_H
