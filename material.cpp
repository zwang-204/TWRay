#include "material.h"
#include "interaction.h"

namespace pbrt{

lambertian::lambertian(const Spectrum& a) : albedo(make_shared<solid_color>(a)) {}
lambertian::lambertian(shared_ptr<texture> a) : albedo(a) {}
bool lambertian::scatter(const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec) const {
            srec.is_specular = false;
            srec.attenuation = albedo->value(si.uv[0], si.uv[1], si.p);
            srec.pdf_ptr = make_shared<cosine_pdf>(Vector3f(si.n));
            return true;
        }
float lambertian::scattering_pdf(
            const Ray& r_in, const SurfaceInteraction& si, const Ray& scattered
        ) const {
            auto cosine = Dot(si.n, Normalize(scattered.direction()));
            return cosine < 0 ? 0 : cosine/Pi;
        }




}