#ifndef MATERIALS_DISNEY_H
#define MATERIALS_DISNEY_H

// materials/disney.h*
#include "material.h"
#include "pbrt.h"

namespace pbrt {

// DisneyMaterial Declarations
class DisneyMaterial : public Material {
  public:
    // DisneyMaterial Public Methods
    DisneyMaterial(const std::shared_ptr<Texture<Spectrum>> &color,
                   const std::shared_ptr<Texture<float>> &metallic,
                   const std::shared_ptr<Texture<float>> &eta,
                   const std::shared_ptr<Texture<float>> &roughness,
                   const std::shared_ptr<Texture<float>> &specularTint,
                   const std::shared_ptr<Texture<float>> &anisotropic,
                   const std::shared_ptr<Texture<float>> &sheen,
                   const std::shared_ptr<Texture<float>> &sheenTint,
                   const std::shared_ptr<Texture<float>> &clearcoat,
                   const std::shared_ptr<Texture<float>> &clearcoatGloss,
                   const std::shared_ptr<Texture<float>> &specTrans,
                   const std::shared_ptr<Texture<Spectrum>> &scatterDistance,
                   bool thin,
                   const std::shared_ptr<Texture<float>> &flatness,
                   const std::shared_ptr<Texture<float>> &diffTrans,
                   const std::shared_ptr<Texture<float>> &bumpMap)
        : color(color),
          metallic(metallic),
          eta(eta),
          roughness(roughness),
          specularTint(specularTint),
          anisotropic(anisotropic),
          sheen(sheen),
          sheenTint(sheenTint),
          clearcoat(clearcoat),
          clearcoatGloss(clearcoatGloss),
          specTrans(specTrans),
          scatterDistance(scatterDistance),
          thin(thin),
          flatness(flatness),
          diffTrans(diffTrans),
          bumpMap(bumpMap) {}
    void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
                                    TransportMode mode,
                                    bool allowMultipleLobes) const;

  private:
    // DisneyMaterial Private Data
    std::shared_ptr<Texture<Spectrum>> color;
    std::shared_ptr<Texture<float>> metallic, eta;
    std::shared_ptr<Texture<float>> roughness, specularTint, anisotropic, sheen;
    std::shared_ptr<Texture<float>> sheenTint, clearcoat, clearcoatGloss;
    std::shared_ptr<Texture<float>> specTrans;
    std::shared_ptr<Texture<Spectrum>> scatterDistance;
    bool thin;
    std::shared_ptr<Texture<float>> flatness, diffTrans, bumpMap;
};

DisneyMaterial *CreateDisneyMaterial(const TextureParams &mp);

}  // namespace pbrt

#endif  // PBRT_MATERIALS_DISNEY_H
