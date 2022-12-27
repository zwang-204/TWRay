#ifndef INTERPOLATION_H
#define INTERPOLATION_H

// core/interpolation.h*
#include "pbrt.h"

namespace pbrt {

// Spline Interpolation Declarations
float CatmullRom(int size, const float *nodes, const float *values, float x);
bool CatmullRomWeights(int size, const float *nodes, float x, int *offset,
                       float *weights);
float SampleCatmullRom(int size, const float *nodes, const float *f,
                       const float *cdf, float sample, float *fval = nullptr,
                       float *pdf = nullptr);
float SampleCatmullRom2D(int size1, int size2, const float *nodes1,
                         const float *nodes2, const float *values,
                         const float *cdf, float alpha, float sample,
                         float *fval = nullptr, float *pdf = nullptr);
float IntegrateCatmullRom(int n, const float *nodes, const float *values,
                          float *cdf);
float InvertCatmullRom(int n, const float *x, const float *values, float u);

// Fourier Interpolation Declarations
float Fourier(const float *a, int m, double cosPhi);
float SampleFourier(const float *ak, const float *recip, int m, float u,
                    float *pdf, float *phiPtr);

}  // namespace pbrt

#endif  // PBRT_CORE_INTERPOLATION_H
