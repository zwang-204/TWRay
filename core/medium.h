#ifndef MEDIUM_H
#define MEDIUM_H

// core/medium.h*
#include "pbrt.h"
#include "geometry.h"
#include "spectrum.h"
#include <memory>

namespace pbrt {

// Media Declarations
class PhaseFunction {
  public:
    // PhaseFunction Interface
    virtual ~PhaseFunction();
    virtual float p(const Vector3f &wo, const Vector3f &wi) const = 0;
    virtual float Sample_p(const Vector3f &wo, Vector3f *wi,
                           const Point2f &u) const = 0;
    virtual std::string ToString() const = 0;
};

inline std::ostream &operator<<(std::ostream &os, const PhaseFunction &p) {
    os << p.ToString();
    return os;
}

bool GetMediumScatteringProperties(const std::string &name, Spectrum *sigma_a,
                                   Spectrum *sigma_s);

// Media Inline Functions
inline float PhaseHG(float cosTheta, float g) {
    float denom = 1 + g * g + 2 * g * cosTheta;
    return Inv4Pi * (1 - g * g) / (denom * std::sqrt(denom));
}

// Medium Declarations
class Medium {
  public:
    // Medium Interface
    virtual ~Medium() {}
    virtual Spectrum Tr(const Ray &ray, Sampler &sampler) const = 0;
    virtual Spectrum Sample(const Ray &ray, Sampler &sampler,
                            MemoryArena &arena,
                            MediumInteraction *mi) const = 0;
};

// HenyeyGreenstein Declarations
class HenyeyGreenstein : public PhaseFunction {
  public:
    // HenyeyGreenstein Public Methods
    HenyeyGreenstein(float g) : g(g) {}
    float p(const Vector3f &wo, const Vector3f &wi) const;
    float Sample_p(const Vector3f &wo, Vector3f *wi,
                   const Point2f &sample) const;
    std::string ToString() const {
        return StringPrintf("[ HenyeyGreenstein g: %f ]", g);
    }

  private:
    const float g;
};

// MediumInterface Declarations
struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}
    // MediumInterface Public Methods
    MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
    MediumInterface(const Medium *inside, const Medium *outside)
        : inside(inside), outside(outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium *inside, *outside;
};

}  // namespace pbrt

#endif  // PBRT_CORE_MEDIUM_H
