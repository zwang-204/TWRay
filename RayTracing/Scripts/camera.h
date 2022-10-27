#ifndef CAMERA_H
#define CAMERA_H

#include "pbrt.h"

namespace pbrt{

class camera {
    public:
        camera(
            Point3f lookfrom,
            Point3f lookat,
            Vector3f vup,
            float vfov,
            float aspect_ratio,
            float aperture,
            float focus_dist,
            float _time0,
            float _time1
        ) {
            auto theta = pbrt::degrees_to_radians(vfov);
            auto h = tan(theta/2);
            float viewport_height = 2.0 * h;
            float viewport_width = aspect_ratio * viewport_height;

            w = Normalize(lookfrom - lookat);
            u = Normalize(Cross(vup, w));
            v = Cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1;
        }

        Ray get_ray(float s, float t) const {
            Vector3f rd = lens_radius * Random_in_unit_disk();
            Vector3f offset = u * rd.x + v * rd.y;
            return Ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }
    
    private:
        Point3f origin;
        Point3f lower_left_corner;
        Vector3f horizontal;
        Vector3f vertical;
        Vector3f u, v, w;
        float lens_radius;
        float time0, time1;
};

}

#endif