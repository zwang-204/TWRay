#ifndef MATERIAL_H
#define MATERIAL_H

#include "pbrt.h"
#include "geometry.h"
#include "texture.h"

namespace pbrt{

struct hit_record;

struct scatter_record {
    Ray specular_ray;
    bool is_specular;
    Vector3f attenuation;
    shared_ptr<pdf> pdf_ptr;
};

class material {
    public:
        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, scatter_record& srec
        ) const {
            return false;
        }

        virtual float scattering_pdf(
            const Ray& r_in, const hit_record& rec, const Ray& scattered
        ) const {
            return 0;
        }

        virtual Vector3f emitted(
                const Ray& r_in, const hit_record& rec, float u, float v, const Point3f& p
            ) const {
            return Vector3f(0,0,0);
        }
        
};

class lambertian : public material {
    public:
        lambertian(const Vector3f& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.is_specular = false;
            srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
            return true;
        }
        float scattering_pdf(
            const Ray& r_in, const hit_record& rec, const Ray& scattered
        ) const {
            auto cosine = Dot(rec.normal, Normalize(scattered.direction()));
            return cosine < 0 ? 0 : cosine/PI;
        }

    public:
        shared_ptr<texture> albedo;
};

class diffuse_light : public material {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(Vector3f c) : emit(make_shared<solid_color>(c)) {}

        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            return false;
        }

        virtual Vector3f emitted(const Ray& r_in, const hit_record& rec, float u, float v, 
            const Point3f& p) const override {
            
            if (rec.front_face)
                return emit->value(u, v, p);
            else
                return Vector3f(0,0,0);
        }

    public:
        shared_ptr<texture> emit;
};

class metal : public material {
    public: 
        metal(const Vector3f& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override{
            Vector3f reflected = Reflect(Normalize(r_in.direction()), rec.normal);
            srec.specular_ray = Ray(rec.p, reflected+fuzz*Random_in_unit_sphere());
            srec.attenuation = albedo;
            srec.is_specular = true;
            srec.pdf_ptr = 0;
            return true;
        }
    
    public:
        Vector3f albedo;
        float fuzz;
};

class dielectric : public material {
    public:
        dielectric(float index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.is_specular = true;
            srec.pdf_ptr = nullptr;
            srec.attenuation = Vector3f(1.0, 1.0, 1.0);
            float refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            Vector3f unit_direction = Normalize(r_in.direction());
            float cos_theta = fminf(Dot(-unit_direction, rec.normal), 1.0);
            float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            Vector3f direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
                direction = Reflect(unit_direction, rec.normal);
            else
                direction = Refract(unit_direction, rec.normal, refraction_ratio);

            srec.specular_ray = Ray(rec.p, direction);
            return true;
        }
    
    public:
        float ir;
    
    private:
        static float reflectance(float cosine, float ref_idx) {
            // Schlick's approximation
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine), 5);
        }
};

/*


class isotropic : public material {
    public:
        isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
    
    public:
        shared_ptr<texture> albedo;
}; 
*/

}

#endif