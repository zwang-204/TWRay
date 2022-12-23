#ifndef MATERIAL_H
#define MATERIAL_H

// core/material.h*
#include "pbrt.h"
#include "memory.h"

namespace pbrt {

// TransportMode Declarations
enum class TransportMode { Radiance, Importance };

// Material Declarations
class Material {
  public:
    // Material Interface
    virtual void ComputeScatteringFunctions(SurfaceInteraction *si,
                                            MemoryArena &arena,
                                            TransportMode mode,
                                            bool allowMultipleLobes) const = 0;
    virtual ~Material();
    static void Bump(const std::shared_ptr<Texture<float>> &d,
                     SurfaceInteraction *si);
};

}  // namespace pbrt

#endif  // PBRT_CORE_MATERIAL_H