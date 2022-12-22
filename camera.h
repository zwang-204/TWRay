#ifndef CAMERA_H
#define CAMERA_H

// core/camera.h*
#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "film.h"
#include "light.h"

namespace pbrt {

// Camera Declarations
class Camera {
  public:
    // Camera Interface
    Camera(const AnimatedTransform &CameraToWorld, float shutterOpen,
           float shutterClose, Film *film);
    virtual ~Camera();
    virtual float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
    virtual float GenerateRayDifferential(const CameraSample &sample,
                                          RayDifferential *rd) const;
    virtual Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
    virtual void Pdf_We(const Ray &ray, float *pdfPos, float *pdfDir) const;
    virtual Spectrum Sample_Wi(const Interaction &ref, const Point2f &u,
                               Vector3f *wi, float *pdf, Point2f *pRaster,
                               VisibilityTester *vis) const;

    // Camera Public Data
    AnimatedTransform CameraToWorld;
    const float shutterOpen, shutterClose;
    Film *film;
};

struct CameraSample {
    Point2f pFilm;
    Point2f pLens;
    float time;
};

inline std::ostream &operator<<(std::ostream &os, const CameraSample &cs) {
    os << "[ pFilm: " << cs.pFilm << " , pLens: " << cs.pLens <<
        StringPrintf(", time %f ]", cs.time);
    return os;
}

class ProjectiveCamera : public Camera {
  public:
    // ProjectiveCamera Public Methods
    ProjectiveCamera(const AnimatedTransform &CameraToWorld,
                     const Transform &CameraToScreen,
                     const Bounds2f &screenWindow, float shutterOpen,
                     float shutterClose, float lensr, float focald, Film *film)
        : Camera(CameraToWorld, shutterOpen, shutterClose, film),
          CameraToScreen(CameraToScreen) {
        // Initialize depth of field parameters
        lensRadius = lensr;
        focalDistance = focald;

        // Compute projective camera transformations

        // Compute projective camera screen transformations
        ScreenToRaster =
            Scale(film->fullResolution.x, film->fullResolution.y, 1) *
            Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                  1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
            Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
        RasterToScreen = Inverse(ScreenToRaster);
        RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
    }

  protected:
    // ProjectiveCamera Protected Data
    Transform CameraToScreen, RasterToCamera;
    Transform ScreenToRaster, RasterToScreen;
    float lensRadius, focalDistance;
};

}  // namespace pbrt

#endif  // PBRT_CORE_CAMERA_H
