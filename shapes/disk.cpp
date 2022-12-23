// shapes/disk.cpp*
#include "disk.h"
#include "../paramset.h"
#include "../sampling.h"
#include "../stats.h"

namespace pbrt {

// Disk Method Definitions
Bounds3f Disk::ObjectBound() const {
    return Bounds3f(Point3f(-radius, -radius, height),
                    Point3f(radius, radius, height));
}

bool Disk::Intersect(const Ray &r, float *tHit, SurfaceInteraction *isect,
                     bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersect);
    // Transform _Ray_ to object space
    Vector3f oErr, dErr;
    Ray ray = (*WorldToObject)(r, &oErr, &dErr);

    // Compute plane intersection for disk

    // Reject disk intersections for rays parallel to the disk's plane
    if (ray.d.z == 0) return false;
    float tShapeHit = (height - ray.o.z) / ray.d.z;
    if (tShapeHit <= 0 || tShapeHit >= ray.tMax) return false;

    // See if hit point is inside disk radii and $\phimax$
    Point3f pHit = ray(tShapeHit);
    float dist2 = pHit.x * pHit.x + pHit.y * pHit.y;
    if (dist2 > radius * radius || dist2 < innerRadius * innerRadius)
        return false;

    // Test disk $\phi$ value against $\phimax$
    float phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0) phi += 2 * Pi;
    if (phi > phiMax) return false;

    // Find parametric representation of disk hit
    float u = phi / phiMax;
    float rHit = std::sqrt(dist2);
    float v = (radius - rHit) / (radius - innerRadius);
    Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0);
    Vector3f dpdv =
        Vector3f(pHit.x, pHit.y, 0.) * (innerRadius - radius) / rHit;
    Normal3f dndu(0, 0, 0), dndv(0, 0, 0);

    // Refine disk intersection point
    pHit.z = height;

    // Compute error bounds for disk intersection
    Vector3f pError(0, 0, 0);

    // Initialize _SurfaceInteraction_ from parametric information
    *isect = (*ObjectToWorld)(SurfaceInteraction(pHit, pError, Point2f(u, v),
                                                 -ray.d, dpdu, dpdv, dndu, dndv,
                                                 ray.time, this));

    // Update _tHit_ for quadric intersection
    *tHit = (float)tShapeHit;
    return true;
}

bool Disk::IntersectP(const Ray &r, bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersectP);
    // Transform _Ray_ to object space
    Vector3f oErr, dErr;
    Ray ray = (*WorldToObject)(r, &oErr, &dErr);

    // Compute plane intersection for disk

    // Reject disk intersections for rays parallel to the disk's plane
    if (ray.d.z == 0) return false;
    float tShapeHit = (height - ray.o.z) / ray.d.z;
    if (tShapeHit <= 0 || tShapeHit >= ray.tMax) return false;

    // See if hit point is inside disk radii and $\phimax$
    Point3f pHit = ray(tShapeHit);
    float dist2 = pHit.x * pHit.x + pHit.y * pHit.y;
    if (dist2 > radius * radius || dist2 < innerRadius * innerRadius)
        return false;

    // Test disk $\phi$ value against $\phimax$
    float phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0) phi += 2 * Pi;
    if (phi > phiMax) return false;
    return true;
}

float Disk::Area() const {
    return phiMax * 0.5 * (radius * radius - innerRadius * innerRadius);
}

Interaction Disk::Sample(const Point2f &u, float *pdf) const {
    Point2f pd = ConcentricSampleDisk(u);
    Point3f pObj(pd.x * radius, pd.y * radius, height);
    Interaction it;
    it.n = Normalize((*ObjectToWorld)(Normal3f(0, 0, 1)));
    if (reverseOrientation) it.n *= -1;
    it.p = (*ObjectToWorld)(pObj, Vector3f(0, 0, 0), &it.pError);
    *pdf = 1 / Area();
    return it;
}

std::shared_ptr<Disk> CreateDiskShape(const Transform *o2w,
                                      const Transform *w2o,
                                      bool reverseOrientation,
                                      const ParamSet &params) {
    float height = params.FindOneFloat("height", 0.);
    float radius = params.FindOneFloat("radius", 1);
    float inner_radius = params.FindOneFloat("innerradius", 0);
    float phimax = params.FindOneFloat("phimax", 360);
    return std::make_shared<Disk>(o2w, w2o, reverseOrientation, height, radius,
                                  inner_radius, phimax);
}

}  // namespace pbrt
