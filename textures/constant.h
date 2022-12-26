#ifndef TEXTURES_CONSTANT_H
#define TEXTURES_CONSTANT_H

// textures/constant.h*
#include "pbrt.h"
#include "texture.h"
#include "paramset.h"

namespace pbrt {

// ConstantTexture Declarations
template <typename T>
class ConstantTexture : public Texture<T> {
  public:
    // ConstantTexture Public Methods
    ConstantTexture(const T &value) : value(value) {}
    T Evaluate(const SurfaceInteraction &) const { return value; }

  private:
    T value;
};

ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
                                                   const TextureParams &tp);
ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
    const Transform &tex2world, const TextureParams &tp);

}  // namespace pbrt

#endif  // PBRT_TEXTURES_CONSTANT_H
