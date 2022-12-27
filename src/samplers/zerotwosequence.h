#ifndef SAMPLERS_ZEROTWOSEQUENCE_H
#define SAMPLERS_ZEROTWOSEQUENCE_H

// samplers/zerotwosequence.h*
#include "sampler.h"

namespace pbrt {

// ZeroTwoSequenceSampler Declarations
class ZeroTwoSequenceSampler : public PixelSampler {
  public:
    // ZeroTwoSequenceSampler Public Methods
    ZeroTwoSequenceSampler(int64_t samplesPerPixel, int nSampledDimensions = 4);
    void StartPixel(const Point2i &);
    std::unique_ptr<Sampler> Clone(int seed);
    int RoundCount(int count) const { return RoundUpPow2(count); }
};

ZeroTwoSequenceSampler *CreateZeroTwoSequenceSampler(const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SAMPLERS_ZEROTWOSEQUENCE_H
