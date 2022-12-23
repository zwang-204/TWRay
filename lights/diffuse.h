#ifndef LIGHTS_DIFFUSE_H
#define LIGHTS_DIFFUSE_H

// lights/diffuse.h*
#include "../pbrt.h"
#include "../light.h"
#include "../primitive.h"

namespace pbrt {

// DiffuseAreaLight Declarations
class DiffuseAreaLight : public AreaLight {
  public:
    // DiffuseAreaLight Public Methods
    DiffuseAreaLight(const Transform &LightToWorld,
                     const MediumInterface &mediumInterface, const Spectrum &Le,
                     int nSamples, const std::shared_ptr<Shape> &shape,
                     bool twoSided = false);
    Spectrum L(const Interaction &intr, const Vector3f &w) const {
        return (twoSided || Dot(intr.n, w) > 0) ? Lemit : Spectrum(0.f);
    }
    Spectrum Power() const;
    Spectrum Sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wo,
                       float *pdf, VisibilityTester *vis) const;
    float Pdf_Li(const Interaction &, const Vector3f &) const;
    Spectrum Sample_Le(const Point2f &u1, const Point2f &u2, float time,
                       Ray *ray, Normal3f *nLight, float *pdfPos,
                       float *pdfDir) const;
    void Pdf_Le(const Ray &, const Normal3f &, float *pdfPos,
                float *pdfDir) const;

  protected:
    // DiffuseAreaLight Protected Data
    const Spectrum Lemit;
    std::shared_ptr<Shape> shape;
    // Added after book publication: by default, DiffuseAreaLights still
    // only emit in the hemimsphere around the surface normal.  However,
    // this behavior can now be overridden to give emission on both sides.
    const bool twoSided;
    const float area;
};

std::shared_ptr<AreaLight> CreateDiffuseAreaLight(
    const Transform &light2world, const Medium *medium,
    const ParamSet &paramSet, const std::shared_ptr<Shape> &shape);

}  // namespace pbrt

#endif  // PBRT_LIGHTS_DIFFUSE_H
