#include "material.h"
#include "interaction.h"

namespace pbrt{

bool lambertian::scatter(const Ray& r_in, const SurfaceInteraction& si, scatter_record& srec) const {
            srec.is_specular = false;
            srec.attenuation = albedo->Evaluate(si);
            srec.pdf_ptr = make_shared<cosine_pdf>(Vector3f(si.n));
            return true;
        }

}