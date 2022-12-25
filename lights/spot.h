#ifndef LIGHTS_SPOT_H
#define LIGHTS_SPOT_H

// lights/spot.h*
#include "../pbrt.h"
#include "../light.h"
#include "../shape.h"

namespace pbrt {

// SpotLight Declarations
class SpotLight : public Light {
  public:
    // SpotLight Public Methods
    SpotLight(const Transform &LightToWorld, const MediumInterface &m,
              const Spectrum &I, float totalWidth, float falloffStart);
    Spectrum Sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi,
                       float *pdf, VisibilityTester *vis) const;
    float Falloff(const Vector3f &w) const;
    Spectrum Power() const;
    float Pdf_Li(const Interaction &, const Vector3f &) const;
    Spectrum Sample_Le(const Point2f &u1, const Point2f &u2, float time,
                       Ray *ray, Normal3f *nLight, float *pdfPos,
                       float *pdfDir) const;
    void Pdf_Le(const Ray &, const Normal3f &, float *pdfPos,
                float *pdfDir) const;

  private:
    // SpotLight Private Data
    const Point3f pLight;
    const Spectrum I;
    const float cosTotalWidth, cosFalloffStart;
};

std::shared_ptr<SpotLight> CreateSpotLight(const Transform &l2w,
                                           const Medium *medium,
                                           const ParamSet &paramSet);

}  // namespace pbrt

#endif  // PBRT_LIGHTS_SPOT_H
