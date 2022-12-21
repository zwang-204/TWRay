#ifndef TEXTURE_H
#define TEXTURE_H

#include "pbrt.h"
#include "spectrum.h"

#include <iostream>

namespace pbrt{

class texture {
    public:
        virtual Spectrum value(float u, float v, const Point3f& p) const = 0;
        return 0;
};

class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(Spectrum c) : color_value(c) {}
        
        virtual Spectrum value(float u, float v, const Point3f& p) const override {
            return color_value;
        }
    
    private:
        Spectrum color_value;
};

// class checker_texture : public texture {
//     public:
//         checker_texture() {}

//         checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
//             : even(_even), odd(_odd) {}

//         checker_texture(Vector3f c1, Vector3f c2)
//             : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}
        
//         virtual Spectrum value(float u, float v, const Point3f& p) const override {
//             auto sines = sin(10*p.x) * sin(10*p.y) * sin(10*p.z);
//             if (sines < 0)
//                 return odd->value(u, v, p);
//             else
//                 return even->value(u, v, p);
//         }
    
//     public:
//         shared_ptr<texture> odd;
//         shared_ptr<texture> even;
// };

}

#endif