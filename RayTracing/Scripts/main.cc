#include "pbrt.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "shapes/sphere.h"
#include "camera.h"
#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "material.h"

#include <iostream>

using namespace pbrt;

Vector3f ray_color(
    const Ray& r,
    const Vector3f& background, 
    const hittable_list& world,
    shared_ptr<hittable> lights, 
    int depth
){
    SurfaceInteraction si;

    if (depth <= 0)
        return Vector3f(0,0,0);

    if (!world.hit(r, 0.001, Infinity, si)) 
        return background;
    scatter_record srec;
    Vector3f emitted = si.mat_ptr->emitted(r, si, si.uv[0], si.uv[1], si.p);
    if(!si.mat_ptr->scatter(r, si, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation
            * ray_color(srec.specular_ray, background, world, lights, depth-1);
    }
    
    auto light_ptr = make_shared<hittable_pdf>(lights, si.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);

    Ray scattered = Ray(si.p, p.generate());
    auto pdf_val = p.value(scattered.direction());

    return emitted + srec.attenuation * si.mat_ptr->scattering_pdf(r, si, scattered)
                            * ray_color(scattered, background, world, lights, depth-1) / pdf_val;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(Vector3f(.65, .05, .05));
    auto white = make_shared<lambertian>(Vector3f(.73, .73, .73));
    auto green = make_shared<lambertian>(Vector3f(.12, .45, .15));
    auto light = make_shared<diffuse_light>(Vector3f(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    
    shared_ptr<hittable> box1 = make_shared<box>(Point3f(0,0,0), Point3f(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, Vector3f(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(Point3f(0,0,0), Point3f(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, Vector3f(130,0,65));
    objects.add(box2);
    //auto glass = make_shared<dielectric>(1.5);
    //objects.add(make_shared<cylinder>(90, point3(190,90,190),0, 90, 2*pi, red));

    return objects;
}

int main(){
    // Image
    auto aspect_ratio = 1.0;
    int image_width = 200;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 30;
    const int max_depth = 5;

    // World
    hittable_list world = cornell_box();
    size_t start = 0;
    size_t end = world.objects.size() -1 ;
    auto world_bvh = bvh_node(world.objects, start, end);
    auto lights = make_shared<hittable_list>();
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    //lights->add(make_shared<cylinder>(90, point3(190,90,190), 0, 90, 2*pi, shared_ptr<material>()));

    // Camera

    Point3f lookfrom(278, 278, -800);
    Point3f lookat(278,278,0);
    Vector3f vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    Vector3f background(0,0,0);

    camera cam(lookfrom, lookat, vup, 40.0, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    
    // Render
    std::cout<<"P3/n"<< image_width << ' ' << image_height << "\n255\n";
    
    for(int j = image_height-1; j>=0; j--){
        std::cerr << "\rScanlines remaining: "<< j <<' '<<std::flush;
        for(int i = 0; i<image_width; i++){
            Vector3f pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s){
                auto u = (i + pbrt::random_float()) / (image_width - 1);
                auto v = (j + pbrt::random_float()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, lights, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr<<"\nDown.\n";
}

