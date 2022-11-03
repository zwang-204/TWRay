#ifndef COLOR_H
#define COLOR_H

#include "geometry.h"

#include <iostream>

namespace pbrt{

void write_color(std::ostream &out, Vector3f pixel_color, int samples_per_pixel){
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
    
    out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << '\n';
}

}

#endif