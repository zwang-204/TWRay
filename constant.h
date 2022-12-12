#ifndef CONSTANT_H
#define CONSTANT_H

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

}  // namespace pbrt

#endif  // PBRT_TEXTURES_CONSTANT_H