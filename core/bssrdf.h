#ifndef BSSRDF_H
#define BSSRDF_H

// core/bssrdf.h*
#include "interaction.h"
#include "reflection.h"
#include "stats.h"

namespace pbrt {

// BSSRDF Utility Declarations
float FresnelMoment1(float invEta);
float FresnelMoment2(float invEta);

// BSSRDF Declarations
class BSSRDF {
  public:
    // BSSRDF Public Methods
    BSSRDF(const SurfaceInteraction &po, float eta) : po(po), eta(eta) {}
    virtual ~BSSRDF() {}

    // BSSRDF Interface
    virtual Spectrum S(const SurfaceInteraction &pi, const Vector3f &wi) = 0;
    virtual Spectrum Sample_S(const Scene &scene, float u1, const Point2f &u2,
                              MemoryArena &arena, SurfaceInteraction *si,
                              float *pdf) const = 0;

  protected:
    // BSSRDF Protected Data
    const SurfaceInteraction &po;
    float eta;
};

class SeparableBSSRDF : public BSSRDF {
    friend class SeparableBSSRDFAdapter;

  public:
    // SeparableBSSRDF Public Methods
    SeparableBSSRDF(const SurfaceInteraction &po, float eta,
                    const Material *material, TransportMode mode)
        : BSSRDF(po, eta),
          ns(po.shading.n),
          ss(Normalize(po.shading.dpdu)),
          ts(Cross(ns, ss)),
          material(material),
          mode(mode) {}
    Spectrum S(const SurfaceInteraction &pi, const Vector3f &wi) {
        ProfilePhase pp(Prof::BSSRDFEvaluation);
        float Ft = FrDielectric(CosTheta(po.wo), 1, eta);
        return (1 - Ft) * Sp(pi) * Sw(wi);
    }
    Spectrum Sw(const Vector3f &w) const {
        float c = 1 - 2 * FresnelMoment1(1 / eta);
        return (1 - FrDielectric(CosTheta(w), 1, eta)) / (c * Pi);
    }
    Spectrum Sp(const SurfaceInteraction &pi) const {
        return Sr(Distance(po.p, pi.p));
    }
    Spectrum Sample_S(const Scene &scene, float u1, const Point2f &u2,
                      MemoryArena &arena, SurfaceInteraction *si,
                      float *pdf) const;
    Spectrum Sample_Sp(const Scene &scene, float u1, const Point2f &u2,
                       MemoryArena &arena, SurfaceInteraction *si,
                       float *pdf) const;
    float Pdf_Sp(const SurfaceInteraction &si) const;

    // SeparableBSSRDF Interface
    virtual Spectrum Sr(float d) const = 0;
    virtual float Sample_Sr(int ch, float u) const = 0;
    virtual float Pdf_Sr(int ch, float r) const = 0;

  private:
    // SeparableBSSRDF Private Data
    const Normal3f ns;
    const Vector3f ss, ts;
    const Material *material;
    const TransportMode mode;
};

class TabulatedBSSRDF : public SeparableBSSRDF {
  public:
    // TabulatedBSSRDF Public Methods
    TabulatedBSSRDF(const SurfaceInteraction &po, const Material *material,
                    TransportMode mode, float eta, const Spectrum &sigma_a,
                    const Spectrum &sigma_s, const BSSRDFTable &table)
        : SeparableBSSRDF(po, eta, material, mode), table(table) {
        sigma_t = sigma_a + sigma_s;
        for (int c = 0; c < Spectrum::nSamples; ++c)
            rho[c] = sigma_t[c] != 0 ? (sigma_s[c] / sigma_t[c]) : 0;
    }
    Spectrum Sr(float distance) const;
    float Pdf_Sr(int ch, float distance) const;
    float Sample_Sr(int ch, float sample) const;

  private:
    // TabulatedBSSRDF Private Data
    const BSSRDFTable &table;
    Spectrum sigma_t, rho;
};

struct BSSRDFTable {
    // BSSRDFTable Public Data
    const int nRhoSamples, nRadiusSamples;
    std::unique_ptr<float[]> rhoSamples, radiusSamples;
    std::unique_ptr<float[]> profile;
    std::unique_ptr<float[]> rhoEff;
    std::unique_ptr<float[]> profileCDF;

    // BSSRDFTable Public Methods
    BSSRDFTable(int nRhoSamples, int nRadiusSamples);
    inline float EvalProfile(int rhoIndex, int radiusIndex) const {
        return profile[rhoIndex * nRadiusSamples + radiusIndex];
    }
};

class SeparableBSSRDFAdapter : public BxDF {
  public:
    // SeparableBSSRDFAdapter Public Methods
    SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), bssrdf(bssrdf) {}
    Spectrum f(const Vector3f &wo, const Vector3f &wi) const {
        Spectrum f = bssrdf->Sw(wi);
        // Update BSSRDF transmission term to account for adjoint light
        // transport
        if (bssrdf->mode == TransportMode::Radiance)
            f *= bssrdf->eta * bssrdf->eta;
        return f;
    }
    std::string ToString() const { return "[ SeparableBSSRDFAdapter ]"; }

  private:
    const SeparableBSSRDF *bssrdf;
};

float BeamDiffusionSS(float sigma_s, float sigma_a, float g, float eta,
                      float r);
float BeamDiffusionMS(float sigma_s, float sigma_a, float g, float eta,
                      float r);
void ComputeBeamDiffusionBSSRDF(float g, float eta, BSSRDFTable *t);
void SubsurfaceFromDiffuse(const BSSRDFTable &table, const Spectrum &rhoEff,
                           const Spectrum &mfp, Spectrum *sigma_a,
                           Spectrum *sigma_s);

}  // namespace pbrt

#endif  // PBRT_CORE_BSSRDF_H
