#ifndef MICROFACET_H
#define MICROFACET_H

// core/microfacet.h*
#include "pbrt.h"
#include "geometry.h"

namespace pbrt {

// MicrofacetDistribution Declarations
class MicrofacetDistribution {
  public:
    // MicrofacetDistribution Public Methods
    virtual ~MicrofacetDistribution();
    virtual float D(const Vector3f &wh) const = 0;
    virtual float Lambda(const Vector3f &w) const = 0;
    float G1(const Vector3f &w) const {
        //    if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
        return 1 / (1 + Lambda(w));
    }
    virtual float G(const Vector3f &wo, const Vector3f &wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi));
    }
    virtual Vector3f Sample_wh(const Vector3f &wo, const Point2f &u) const = 0;
    float Pdf(const Vector3f &wo, const Vector3f &wh) const;
    virtual std::string ToString() const = 0;

  protected:
    // MicrofacetDistribution Protected Methods
    MicrofacetDistribution(bool sampleVisibleArea)
        : sampleVisibleArea(sampleVisibleArea) {}

    // MicrofacetDistribution Protected Data
    const bool sampleVisibleArea;
};

inline std::ostream &operator<<(std::ostream &os,
                                const MicrofacetDistribution &md) {
    os << md.ToString();
    return os;
}

class BeckmannDistribution : public MicrofacetDistribution {
  public:
    // BeckmannDistribution Public Methods
    static float RoughnessToAlpha(float roughness) {
        roughness = std::max(roughness, (float)1e-3);
        float x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }
    BeckmannDistribution(float alphax, float alphay, bool samplevis = true)
        : MicrofacetDistribution(samplevis),
          alphax(std::max(float(0.001), alphax)),
          alphay(std::max(float(0.001), alphay)) {}
    float D(const Vector3f &wh) const;
    Vector3f Sample_wh(const Vector3f &wo, const Point2f &u) const;
    std::string ToString() const;

  private:
    // BeckmannDistribution Private Methods
    float Lambda(const Vector3f &w) const;

    // BeckmannDistribution Private Data
    const float alphax, alphay;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
  public:
    // TrowbridgeReitzDistribution Public Methods
    static inline float RoughnessToAlpha(float roughness);
    TrowbridgeReitzDistribution(float alphax, float alphay,
                                bool samplevis = true)
        : MicrofacetDistribution(samplevis),
          alphax(std::max(float(0.001), alphax)),
          alphay(std::max(float(0.001), alphay)) {}
    float D(const Vector3f &wh) const;
    Vector3f Sample_wh(const Vector3f &wo, const Point2f &u) const;
    std::string ToString() const;

  private:
    // TrowbridgeReitzDistribution Private Methods
    float Lambda(const Vector3f &w) const;

    // TrowbridgeReitzDistribution Private Data
    const float alphax, alphay;
};

// MicrofacetDistribution Inline Methods
inline float TrowbridgeReitzDistribution::RoughnessToAlpha(float roughness) {
    roughness = std::max(roughness, (float)1e-3);
    float x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
           0.000640711f * x * x * x * x;
}

}  // namespace pbrt

#endif  // PBRT_CORE_MICROFACET_H
