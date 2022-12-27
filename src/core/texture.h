#ifndef TEXTURE_H
#define TEXTURE_H

// core/texture.h*
#include "pbrt.h"
#include "spectrum.h"
#include "geometry.h"
#include "transform.h"
#include "memory.h"

#include "textures/constant.h"

namespace pbrt {

// Texture Declarations
class TextureMapping2D {
  public:
    // TextureMapping2D Interface
    virtual ~TextureMapping2D();
    virtual Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
                        Vector2f *dstdy) const = 0;
};

class UVMapping2D : public TextureMapping2D {
  public:
    // UVMapping2D Public Methods
    UVMapping2D(float su = 1, float sv = 1, float du = 0, float dv = 0);
    Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
                Vector2f *dstdy) const;

  private:
    const float su, sv, du, dv;
};

class SphericalMapping2D : public TextureMapping2D {
  public:
    // SphericalMapping2D Public Methods
    SphericalMapping2D(const Transform &WorldToTexture)
        : WorldToTexture(WorldToTexture) {}
    Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
                Vector2f *dstdy) const;

  private:
    Point2f sphere(const Point3f &P) const;
    const Transform WorldToTexture;
};

class CylindricalMapping2D : public TextureMapping2D {
  public:
    // CylindricalMapping2D Public Methods
    CylindricalMapping2D(const Transform &WorldToTexture)
        : WorldToTexture(WorldToTexture) {}
    Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
                Vector2f *dstdy) const;

  private:
    // CylindricalMapping2D Private Methods
    Point2f cylinder(const Point3f &p) const {
        Vector3f vec = Normalize(WorldToTexture(p) - Point3f(0, 0, 0));
        return Point2f((Pi + std::atan2(vec.y, vec.x)) * Inv2Pi, vec.z);
    }
    const Transform WorldToTexture;
};

class PlanarMapping2D : public TextureMapping2D {
  public:
    // PlanarMapping2D Public Methods
    Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
                Vector2f *dstdy) const;
    PlanarMapping2D(const Vector3f &vs, const Vector3f &vt, float ds = 0,
                    float dt = 0)
        : vs(vs), vt(vt), ds(ds), dt(dt) {}

  private:
    const Vector3f vs, vt;
    const float ds, dt;
};

class TextureMapping3D {
  public:
    // TextureMapping3D Interface
    virtual ~TextureMapping3D();
    virtual Point3f Map(const SurfaceInteraction &si, Vector3f *dpdx,
                        Vector3f *dpdy) const = 0;
};

class IdentityMapping3D : public TextureMapping3D {
  public:
    // IdentityMapping3D Public Methods
    IdentityMapping3D(const Transform &WorldToTexture)
        : WorldToTexture(WorldToTexture) {}
    Point3f Map(const SurfaceInteraction &si, Vector3f *dpdx,
                Vector3f *dpdy) const;

  private:
    const Transform WorldToTexture;
};

template <typename T>
class Texture {
  public:
    // Texture Interface
    virtual T Evaluate(const SurfaceInteraction &) const = 0;
    virtual ~Texture() {}
};

float Lanczos(float, float tau = 2);
float Noise(float x, float y = .5f, float z = .5f);
float Noise(const Point3f &p);
float FBm(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
          float omega, int octaves);
float Turbulence(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
                 float omega, int octaves);

}  // namespace pbrt

#endif  // PBRT_CORE_TEXTURE_H
