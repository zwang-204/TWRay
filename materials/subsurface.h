#ifndef MATERIALS_SUBSURFACE_H
#define MATERIALS_SUBSURFACE_H

// materials/subsurface.h*
#include "../pbrt.h"
#include "../material.h"
#include "../reflection.h"
#include "../bssrdf.h"

namespace pbrt {

// SubsurfaceMaterial Declarations
class SubsurfaceMaterial : public Material {
  public:
    // SubsurfaceMaterial Public Methods
    SubsurfaceMaterial(float scale,
                       const std::shared_ptr<Texture<Spectrum>> &Kr,
                       const std::shared_ptr<Texture<Spectrum>> &Kt,
                       const std::shared_ptr<Texture<Spectrum>> &sigma_a,
                       const std::shared_ptr<Texture<Spectrum>> &sigma_s,
                       float g, float eta,
                       const std::shared_ptr<Texture<float>> &uRoughness,
                       const std::shared_ptr<Texture<float>> &vRoughness,
                       const std::shared_ptr<Texture<float>> &bumpMap,
                       bool remapRoughness)
        : scale(scale),
          Kr(Kr),
          Kt(Kt),
          sigma_a(sigma_a),
          sigma_s(sigma_s),
          uRoughness(uRoughness),
          vRoughness(vRoughness),
          bumpMap(bumpMap),
          eta(eta),
          remapRoughness(remapRoughness),
          table(100, 64) {
        ComputeBeamDiffusionBSSRDF(g, eta, &table);
    }
    void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
                                    TransportMode mode,
                                    bool allowMultipleLobes) const;

  private:
    // SubsurfaceMaterial Private Data
    const float scale;
    std::shared_ptr<Texture<Spectrum>> Kr, Kt, sigma_a, sigma_s;
    std::shared_ptr<Texture<float>> uRoughness, vRoughness;
    std::shared_ptr<Texture<float>> bumpMap;
    const float eta;
    const bool remapRoughness;
    BSSRDFTable table;
};

SubsurfaceMaterial *CreateSubsurfaceMaterial(const TextureParams &mp);

}  // namespace pbrt

#endif  // PBRT_MATERIALS_SUBSURFACE_H
