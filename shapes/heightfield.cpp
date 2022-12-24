// shapes/heightfield.cpp*
#include "heightfield.h"
#include "triangle.h"
#include "../paramset.h"

namespace pbrt {

// Heightfield Definitions
std::vector<std::shared_ptr<Shape>> CreateHeightfield(
    const Transform *ObjectToWorld, const Transform *WorldToObject,
    bool reverseOrientation, const ParamSet &params) {
    int nx = params.FindOneInt("nu", -1);
    int ny = params.FindOneInt("nv", -1);
    int nitems;
    const float *z = params.FindFloat("Pz", &nitems);
    CHECK_EQ(nitems, nx * ny);
    CHECK(nx != -1 && ny != -1 && z != nullptr);

    int ntris = 2 * (nx - 1) * (ny - 1);
    std::unique_ptr<int[]> indices(new int[3 * ntris]);
    std::unique_ptr<Point3f[]> P(new Point3f[nx * ny]);
    std::unique_ptr<Point2f[]> uvs(new Point2f[nx * ny]);
    int nverts = nx * ny;
    // Compute heightfield vertex positions
    int pos = 0;
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            P[pos].x = uvs[pos].x = (float)x / (float)(nx - 1);
            P[pos].y = uvs[pos].y = (float)y / (float)(ny - 1);
            P[pos].z = z[pos];
            ++pos;
        }
    }

    // Fill in heightfield vertex offset array
    int *vp = indices.get();
    for (int y = 0; y < ny - 1; ++y) {
        for (int x = 0; x < nx - 1; ++x) {
#define VERT(x, y) ((x) + (y)*nx)
            *vp++ = VERT(x, y);
            *vp++ = VERT(x + 1, y);
            *vp++ = VERT(x + 1, y + 1);

            *vp++ = VERT(x, y);
            *vp++ = VERT(x + 1, y + 1);
            *vp++ = VERT(x, y + 1);
        }
#undef VERT
    }

    return CreateTriangleMesh(ObjectToWorld, WorldToObject, reverseOrientation,
                              ntris, indices.get(), nverts, P.get(), nullptr,
                              nullptr, uvs.get(), nullptr, nullptr);
}

}  // namespace pbrt
