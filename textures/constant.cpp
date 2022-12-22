#include "constant.h"

namespace pbrt {

// ConstantTexture Method Definitions
ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
                                                   const TextureParams &tp) {
    return new ConstantTexture<float>(tp.FindFloat("value", 1.f));
}

ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
    const Transform &tex2world, const TextureParams &tp) {
    return new ConstantTexture<Spectrum>(
        tp.FindSpectrum("value", Spectrum(1.f)));
}

}  // namespace pbrt