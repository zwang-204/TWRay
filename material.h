#ifndef MATERIAL_H
#define MATERIAL_H

#include "pbrt.h"
#include "spectrum.h"
#include "geometry.h"
#include "texture.h"
#include "pdf.h"

namespace pbrt{
    
enum class TransportMode { Radiance, Importance };

struct scatter_record {
    Ray specular_ray;
    bool is_specular;
    Spectrum attenuation;
    shared_ptr<pdf> pdf_ptr;
};

class Material {
    public:
        virtual bool scatter(
            const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec
        ) const = 0;

        // virtual float scattering_pdf(
        //     const Ray& r_in, const SurfaceInteraction& si, const Ray& scattered
        // ) const = 0;

        virtual Spectrum emitted(
                const Ray& r_in, const SurfaceInteraction& si, float u, float v, const Point3f& p
            ) const = 0;
        
};

class lambertian : public Material {
    public:
        lambertian(const Spectrum& a);
        lambertian(shared_ptr<texture> a);

        virtual bool scatter(
            const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec
        ) const;
        virtual float scattering_pdf(
            const Ray& r_in, const SurfaceInteraction& si, const Ray& scattered
        ) const;

    public:
        shared_ptr<texture> albedo;
};

class diffuse_light : public Material {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(Spectrum c) : emit(make_shared<solid_color>(c)) {}

        virtual bool scatter(
            const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec
        ) const override {
            return false;
        }

        virtual Spectrum emitted(const Ray& r_in, const SurfaceInteraction& si, float u, float v, 
            const Point3f& p) const override {
            return emit->value(u, v, p);
            // if (si.front_face)
            //     return emit->value(u, v, p);
            // else
            //     return Spectrum(0.0);
        }

    public:
        shared_ptr<texture> emit;
};

// class metal : public material {
//     public: 
//         metal(const Spectrum& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
//         virtual bool scatter(
//             const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec
//         ) const override{
//             Vector3f reflected = Reflect(Normalize(r_in.direction()), si.n);
//             srec.specular_ray = Ray(si.p, reflected+fuzz*Random_in_unit_sphere());
//             srec.attenuation = albedo;
//             srec.is_specular = true;
//             srec.pdf_ptr = 0;
//             return true;
//         }
    
//     public:
//         Spectrum albedo;
//         float fuzz;
// };

// class dielectric : public material {
//     public:
//         dielectric(float index_of_refraction) : ir(index_of_refraction) {}

//         virtual bool scatter(
//             const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec
//         ) const override {
//             srec.is_specular = true;
//             srec.pdf_ptr = nullptr;
//             srec.attenuation = Spectrum(1.0);
//             // float refraction_ratio = si.front_face ? (1.0/ir) : ir;
//             float refraction_ratio = (1.0/ir);

//             Vector3f unit_direction = Normalize(r_in.direction());
//             float cos_theta = fminf(Dot(-unit_direction, si.n), 1.0);
//             float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

//             bool cannot_refract = refraction_ratio * sin_theta > 1.0;
//             Vector3f direction;

//             if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
//                 direction = Reflect(unit_direction, si.n);
//             else
//                 direction = Refract(unit_direction, si.n, refraction_ratio);

//             srec.specular_ray = Ray(si.p, direction);
//             return true;
//         }
    
//     public:
//         float ir;
    
//     private:
//         static float reflectance(float cosine, float ref_idx) {
//             // Schlick's approximation
//             auto r0 = (1-ref_idx) / (1+ref_idx);
//             r0 = r0*r0;
//             return r0 + (1-r0)*pow((1 - cosine), 5);
//         }
// };

}

#endif