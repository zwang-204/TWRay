// core/material.cpp*
#include "material.h"
#include "primitive.h"
#include "texture.h"
#include "spectrum.h"
#include "reflection.h"

namespace pbrt {

// Material Method Definitions
Material::~Material() {}

void Material::Bump(const std::shared_ptr<Texture<float>> &d,
                    SurfaceInteraction *si) {
    // Compute offset positions and evaluate displacement texture
    SurfaceInteraction siEval = *si;

    // Shift _siEval_ _du_ in the $u$ direction
    float du = .5f * (std::abs(si->dudx) + std::abs(si->dudy));
    // The most common reason for du to be zero is for ray that start from
    // light sources, where no differentials are available. In this case,
    // we try to choose a small enough du so that we still get a decently
    // accurate bump value.
    if (du == 0) du = .0005f;
    siEval.p = si->p + du * si->shading.dpdu;
    siEval.uv = si->uv + Vector2f(du, 0.f);
    siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
                         du * si->dndu);
    float uDisplace = d->Evaluate(siEval);

    // Shift _siEval_ _dv_ in the $v$ direction
    float dv = .5f * (std::abs(si->dvdx) + std::abs(si->dvdy));
    if (dv == 0) dv = .0005f;
    siEval.p = si->p + dv * si->shading.dpdv;
    siEval.uv = si->uv + Vector2f(0.f, dv);
    siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
                         dv * si->dndv);
    float vDisplace = d->Evaluate(siEval);
    float displace = d->Evaluate(*si);

    // Compute bump-mapped differential geometry
    Vector3f dpdu = si->shading.dpdu +
                    (uDisplace - displace) / du * Vector3f(si->shading.n) +
                    displace * Vector3f(si->shading.dndu);
    Vector3f dpdv = si->shading.dpdv +
                    (vDisplace - displace) / dv * Vector3f(si->shading.n) +
                    displace * Vector3f(si->shading.dndv);
    si->SetShadingGeometry(dpdu, dpdv, si->shading.dndu, si->shading.dndv,
                           false);
}

}  // namespace pbrt
