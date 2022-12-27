#ifndef SAMPLERS_RANDOM_H
#define SAMPLERS_RANDOM_H

// samplers/random.h*
#include "sampler.h"
#include "rng.h"

namespace pbrt {

class RandomSampler : public Sampler {
  public:
    RandomSampler(int ns, int seed = 0);
    void StartPixel(const Point2i &);
    float Get1D();
    Point2f Get2D();
    std::unique_ptr<Sampler> Clone(int seed);

  private:
    RNG rng;
};

Sampler *CreateRandomSampler(const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SAMPLERS_RANDOM_H