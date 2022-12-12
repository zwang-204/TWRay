#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>
#include <glog/logging.h>
#include <float.h>
#include <alloca.h> 
#include <stdint.h>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

#ifdef _MSC_VER
#define MachineEpsilon (std::numeric_limits<Float>::epsilon() * 0.5)
#else
static constexpr float MachineEpsilon =
    std::numeric_limits<float>::epsilon() * 0.5;
#endif

#ifndef PBRT_L1_CACHE_LINE_SIZE
  #define PBRT_L1_CACHE_LINE_SIZE 64
#endif

static constexpr float ShadowEpsilon = 0.0001f;

const float Infinity = std::numeric_limits<float>::infinity();
const float Pi = 3.1415926535897932385;
const float InvPi = 0.31830988618379067154;
const float Inv2Pi = 0.15915494309189533577;
const float Inv4Pi = 0.07957747154594766788;
const float PiOver2 = 1.57079632679489661923;
const float PiOver4 = 0.78539816339744830961;
const float Sqrt2 = 1.41421356237309504880;


namespace pbrt {

// Global Forward Declarations
class Scene;
class Integrator;
class SamplerIntegrator;
template <typename T>
class Vector2;
template <typename T>
class Vector3;
template <typename T>
class Point3;
template <typename T>
class Point2;
template <typename T>
class Normal3;
class Ray;
class RayDifferential;
template <typename T>
class Bounds2;
template <typename T>
class Bounds3;
class Transform;
struct Interaction;
class SurfaceInteraction;
class Shape;
class Primitive;
class GeometricPrimitive;
class TransformedPrimitive;
template <int nSpectrumSamples>
class CoefficientSpectrum;
class RGBSpectrum;
class SampledSpectrum;
struct Quaternion;
// #ifdef PBRT_SAMPLED_SPECTRUM
//   typedef SampledSpectrum Spectrum;
// #else
//   typedef RGBSpectrum Spectrum;
// #endif
typedef RGBSpectrum Spectrum;


struct Options {
    Options() {
        cropWindow[0][0] = 0;
        cropWindow[0][1] = 1;
        cropWindow[1][0] = 0;
        cropWindow[1][1] = 1;
    }
    int nThreads = 0;
    bool quickRender = false;
    bool quiet = false;
    bool cat = false, toPly = false;
    std::string imageFile;
    // x0, x1, y0, y1
    float cropWindow[2][2];
};

extern Options PbrtOptions;

template <typename T>
inline bool isNaN(const T x) {
    return std::isnan(x);
}
template <>
inline bool isNaN(const int x) {
    return false;
}
template<typename T, typename U>
inline bool isUnequal(const T x, const U y){
    return x!=y;
}

template<typename T, typename U>
inline bool isEqual(const T x, const U y){
    return x==y;
}

inline float degrees_to_radians (float degrees) {
    return degrees * Pi / 180.0;
}

inline float random_float(){
    // Returns a random real in [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max){
    return min + (max-min)*random_float();
}

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}

inline int random_int(int min, int max){
    return static_cast<int>(random_float(min, max+1));
}

inline float Radians(float deg) { return (Pi / 180) * deg; }

inline float Degrees(float rad) { return (180 / Pi) * rad; }

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


template <typename Predicate>
int FindInterval(int size, const Predicate &pred) {
    int first = 0, len = size;
    while (len > 0) {
        int half = len >> 1, middle = first + half;
        // Bisect range based on value of _pred_ at _middle_
        if (pred(middle)) {
            first = middle + 1;
            len -= half + 1;
        } else
            len = half;
    }
    return Clamp(first - 1, 0, size - 2);
}

inline int CountTrailingZeros(uint32_t v) {
#if defined(PBRT_IS_MSVC)
    unsigned long index;
    if (_BitScanForward(&index, v))
        return index;
    else
        return 32;
#else
    return __builtin_ctz(v);
#endif
}


}

// #include "spectrum.h"
// #include "geometry.h"
// #include "onb.h"
// #include "interaction.h"
// #include "pdf.h"
// #include "memory.h"
// #include "paramset.h"
// #include "stats.h"
// #include "rng.h"
// #include "bvh.h"
// #include "primitive.h"
// #include "color.h"
// #include "hittable_list.h"
// #include "sphere.h"
// #include "camera.h"
// #include "aarect.h"
// #include "box.h"
// #include "material.h"


#endif