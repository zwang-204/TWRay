#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

inline float degrees_to_radians (float degrees) {
    return degrees * pi / 180.0;
}

inline float random_float(){
    // Returns a random real in [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max){
    return min + (max-min)*random_float();
}

inline float clamp(float x, float min, float max){
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int random_int(int min, int max){
    return static_cast<int>(random_float(min, max+1));
}

#include "ray.h"
#include "vec3.h"
#include "onb.h"
#include "pdf.h"

#endif