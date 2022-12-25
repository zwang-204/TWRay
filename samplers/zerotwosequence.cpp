// samplers/zerotwosequence.cpp*
#include "zerotwosequence.h"
#include "../lowdiscrepancy.h"
#include "../paramset.h"
#include "../stats.h"

namespace pbrt {

// ZeroTwoSequenceSampler Method Definitions
ZeroTwoSequenceSampler::ZeroTwoSequenceSampler(int64_t samplesPerPixel,
                                               int nSampledDimensions)
    : PixelSampler(RoundUpPow2(samplesPerPixel), nSampledDimensions) {
    if (!IsPowerOf2(samplesPerPixel))
        Warning(
            "Pixel samples being rounded up to power of 2 "
            "(from %" PRId64 " to %" PRId64 ").",
            samplesPerPixel, RoundUpPow2(samplesPerPixel));
}

void ZeroTwoSequenceSampler::StartPixel(const Point2i &p) {
    ProfilePhase _(Prof::StartPixel);
    // Generate 1D and 2D pixel sample components using $(0,2)$-sequence
    for (size_t i = 0; i < samples1D.size(); ++i)
        VanDerCorput(1, samplesPerPixel, &samples1D[i][0], rng);
    for (size_t i = 0; i < samples2D.size(); ++i)
        Sobol2D(1, samplesPerPixel, &samples2D[i][0], rng);

    // Generate 1D and 2D array samples using $(0,2)$-sequence
    for (size_t i = 0; i < samples1DArraySizes.size(); ++i)
        VanDerCorput(samples1DArraySizes[i], samplesPerPixel,
                     &sampleArray1D[i][0], rng);
    for (size_t i = 0; i < samples2DArraySizes.size(); ++i)
        Sobol2D(samples2DArraySizes[i], samplesPerPixel, &sampleArray2D[i][0],
                rng);
    PixelSampler::StartPixel(p);
}

std::unique_ptr<Sampler> ZeroTwoSequenceSampler::Clone(int seed) {
    ZeroTwoSequenceSampler *lds = new ZeroTwoSequenceSampler(*this);
    lds->rng.SetSequence(seed);
    return std::unique_ptr<Sampler>(lds);
}

ZeroTwoSequenceSampler *CreateZeroTwoSequenceSampler(const ParamSet &params) {
    int nsamp = params.FindOneInt("pixelsamples", 16);
    int sd = params.FindOneInt("dimensions", 4);
    // if (PbrtOptions.quickRender) nsamp = 1;
    return new ZeroTwoSequenceSampler(nsamp, sd);
}

}  // namespace pbrt
