#ifndef SAMPLING_H
#define SAMPLING_H

#include "pbrt.h"
#include "geometry.h"
#include <algorithm>

namespace pbrt {

float UniformConePdf(float cosThetaMax) {
    return 1 / (2 * PI * (1 - cosThetaMax));
}

}  // namespace pbrt

#endif