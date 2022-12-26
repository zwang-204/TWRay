#ifndef LIGHTDISTRIB_H
#define LIGHTDISTRIB_H

#include "pbrt.h"
#include "geometry.h"
#include "sampling.h"
#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace pbrt {

// LightDistribution defines a general interface for classes that provide
// probability distributions for sampling light sources at a given point in
// space.
class LightDistribution {
  public:
    virtual ~LightDistribution();

    // Given a point |p| in space, this method returns a (hopefully
    // effective) sampling distribution for light sources at that point.
    virtual const Distribution1D *Lookup(const Point3f &p) const = 0;
};

std::unique_ptr<LightDistribution> CreateLightSampleDistribution(
    const std::string &name, const Scene &scene);

// The simplest possible implementation of LightDistribution: this returns
// a uniform distribution over all light sources, ignoring the provided
// point. This approach works well for very simple scenes, but is quite
// ineffective for scenes with more than a handful of light sources. (This
// was the sampling method originally used for the PathIntegrator and the
// VolPathIntegrator in the printed book, though without the
// UniformLightDistribution class.)
class UniformLightDistribution : public LightDistribution {
  public:
    UniformLightDistribution(const Scene &scene);
    const Distribution1D *Lookup(const Point3f &p) const;

  private:
    std::unique_ptr<Distribution1D> distrib;
};

// PowerLightDistribution returns a distribution with sampling probability
// proportional to the total emitted power for each light. (It also ignores
// the provided point |p|.)  This approach works well for scenes where
// there the most powerful lights are also the most important contributors
// to lighting in the scene, but doesn't do well if there are many lights
// and if different lights are relatively important in some areas of the
// scene and unimportant in others. (This was the default sampling method
// used for the BDPT integrator and MLT integrator in the printed book,
// though also without the PowerLightDistribution class.)
class PowerLightDistribution : public LightDistribution {
  public:
    PowerLightDistribution(const Scene &scene);
    const Distribution1D *Lookup(const Point3f &p) const;

  private:
    std::unique_ptr<Distribution1D> distrib;
};

// A spatially-varying light distribution that adjusts the probability of
// sampling a light source based on an estimate of its contribution to a
// region of space.  A fixed voxel grid is imposed over the scene bounds
// and a sampling distribution is computed as needed for each voxel.
class SpatialLightDistribution : public LightDistribution {
  public:
    SpatialLightDistribution(const Scene &scene, int maxVoxels = 64);
    ~SpatialLightDistribution();
    const Distribution1D *Lookup(const Point3f &p) const;

  private:
    // Compute the sampling distribution for the voxel with integer
    // coordiantes given by "pi".
    Distribution1D *ComputeDistribution(Point3i pi) const;

    const Scene &scene;
    int nVoxels[3];

    // The hash table is a fixed number of HashEntry structs (where we
    // allocate more than enough entries in the SpatialLightDistribution
    // constructor). During rendering, the table is allocated without
    // locks, using atomic operations. (See the Lookup() method
    // implementation for details.)
    struct HashEntry {
        std::atomic<uint64_t> packedPos;
        std::atomic<Distribution1D *> distribution;
    };
    mutable std::unique_ptr<HashEntry[]> hashTable;
    size_t hashTableSize;
};

}  // namespace pbrt

#endif  // PBRT_CORE_LIGHTDISTRIB_H
