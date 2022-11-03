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

const float Infinity = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385;

namespace pbrt {

inline float degrees_to_radians (float degrees) {
    return degrees * PI / 180.0;
}

inline float random_float(){
    // Returns a random real in [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max){
    return min + (max-min)*random_float();
}

inline float Clamp(float x, float min, float max){
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int random_int(int min, int max){
    return static_cast<int>(random_float(min, max+1));
}

inline float Radians(float deg) { return (PI / 180) * deg; }

inline float Degrees(float rad) { return (180 / PI) * rad; }

inline float Lerp(float t, float v1, float v2) { 
    return (1 - t) * v1 + t * v2; 
}

inline uint32_t FloatToBits(float f) {
    uint32_t ui;
    memcpy(&ui, &f, sizeof(float));
    return ui;
}

inline float BitsToFloat(uint32_t ui) {
    float f;
    memcpy(&f, &ui, sizeof(uint32_t));
    return f;
}

inline float NextFloatUp(float v) {
    // Handle infinity and negative zero for _NextFloatUp()_
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;

    // Advance _v_ to next higher float
    uint32_t ui = FloatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
    // Handle infinity and positive zero for _NextFloatDown()_
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint32_t ui = FloatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return BitsToFloat(ui);
}


}

#include "geometry.h"
#include "onb.h"
#include "interaction.h"
#include "pdf.h"
#include "memory.h"

#endif