#ifndef FLOATFILE_H
#define FLOATFILE_H

// core/floatfile.h*
#include "pbrt.h"

namespace pbrt {

bool ReadFloatFile(const char *filename, std::vector<float> *values);

}  // namespace pbrt

#endif  