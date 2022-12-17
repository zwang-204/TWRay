#include "material.h"
#include "interaction.h"

namespace pbrt{

bool lambertian::scatter(const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec) const {
            srec.is_specular = false;
            srec.attenuation = albedo->value(si.uv[0], si.uv[1], si.p);
            srec.pdf_ptr = make_shared<cosine_pdf>(Vector3f(si.n));
            return true;
        }

}