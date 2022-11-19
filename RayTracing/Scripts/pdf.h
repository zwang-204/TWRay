#ifndef PDF_H
#define PDF_H

#include "pbrt.h"
#include "hittable.h"

namespace pbrt{

class pdf {
    public:
        virtual ~pdf() {}

        virtual float value(const Vector3f& direction) const = 0;
        virtual Vector3f generate() const = 0;
};

inline Vector3f random_cosine_direction() {
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = sqrt(1-r2);

    auto phi = 2*PI*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);

    return Vector3f(x, y, z);
}

inline Vector3f random_to_sphere(float radius, float distance_squared) {
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

    auto phi = 2*PI*r1;
    auto x = cos(phi)*sqrt(1-z*z);
    auto y = sin(phi)*sqrt(1-z*z);

    return Vector3f(x, y, z);
}

class cosine_pdf : public pdf {
    public:
        cosine_pdf(const Vector3f& w) { uvw.build_from_w(w); }

        virtual float value(const Vector3f& direction) const override {
            auto cosine = Dot(Normalize(direction), uvw.w());
            return (cosine <= 0) ? 0 : cosine/PI;
        }

        virtual Vector3f generate() const override {
            return uvw.local(random_cosine_direction());
        }

    public:
        onb uvw;
};

class hittable_pdf : public pdf {
    public:
        hittable_pdf(shared_ptr<hittable> p, const Point3f& origin) : ptr(p), o(origin) {}

        virtual float value(const Vector3f& direction) const override {
            return ptr->pdf_value(o, direction);
        }

        virtual Vector3f generate() const override {
            return ptr->random(o);
        }

    public:
        Point3f o;
        shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf {
    public:
        mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
            p[0] = p0;
            p[1] = p1;
        }

        virtual float value(const Vector3f& direction) const override {
            return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction) + 1E-5;
        }

        virtual Vector3f generate() const override {
            if (random_float() < 0.5)
                return p[0]->generate();
            else
                return p[1]->generate();
        }
    
    public:
        shared_ptr<pdf> p[2];
};

}

#endif