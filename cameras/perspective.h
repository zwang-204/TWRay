#ifndef PBRT_CAMERAS_PERSPECTIVE_H
#define PBRT_CAMERAS_PERSPECTIVE_H

// cameras/perspective.h*
#include "../pbrt.h"
#include "../camera.h"
#include "../film.h"

namespace pbrt {

// PerspectiveCamera Declarations
class PerspectiveCamera : public ProjectiveCamera {
  public:
    // PerspectiveCamera Public Methods
    PerspectiveCamera(const AnimatedTransform &CameraToWorld,
                      const Bounds2f &screenWindow, float shutterOpen,
                      float shutterClose, float lensRadius, float focalDistance,
                      float fov, Film *film);
    float GenerateRay(const CameraSample &sample, Ray *) const;
    float GenerateRayDifferential(const CameraSample &sample,
                                  RayDifferential *ray) const;
    Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
    void Pdf_We(const Ray &ray, float *pdfPos, float *pdfDir) const;
    Spectrum Sample_Wi(const Interaction &ref, const Point2f &sample,
                       Vector3f *wi, float *pdf, Point2f *pRaster,
                       VisibilityTester *vis) const;

  private:
    // PerspectiveCamera Private Data
    Vector3f dxCamera, dyCamera;
    float A;
};

PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
                                           const AnimatedTransform &cam2world,
                                           Film *film);

}  // namespace pbrt

#endif  // PBRT_CAMERAS_PERSPECTIVE_H