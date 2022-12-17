#include "pbrt.h"
#include "spectrum.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include "color.h"

#include "paramset.h"
//#include "box.h"
#include "primitive.h"
#include "camera.h"
#include "bvh.h"
#include "sphere.h"

#include "stats.h"
#include "parallel.h"

#include <iostream>
#include <map>

using namespace pbrt;

Spectrum ray_color(
    const Ray& r,
    const Spectrum& background, 
    const shared_ptr<Primitive>& world,
    int depth
){
    SurfaceInteraction si;

    if (depth <= 0)
        return Spectrum(0.0);
    if (!world->Intersect(r, &si)) 
        return background;
    scatter_record srec;
    Spectrum emitted = si.primitive->GetMaterial()->emitted(r, si, si.uv[0], si.uv[1], si.p);
    if(!si.primitive->GetMaterial()->scatter(r, si, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation
            * ray_color(srec.specular_ray, background, world, depth-1);
    }
    
    // auto light_ptr = make_shared<hittable_pdf>(lights, si.p);
    // mixture_pdf p(light_ptr, srec.pdf_ptr);

    Ray scattered = Ray(si.p, Random_in_unit_sphere());
    // auto pdf_val = p.value(scattered.direction());

    return emitted + srec.attenuation * si.primitive->GetMaterial()->scatter(r, si, srec)
                            * ray_color(scattered, background, world, depth-1);
}

std::shared_ptr<GeometricPrimitive> add_light_sphere(Vector3f pos, float radius, float lightCol[3]){
    ParamSet paramSet;
    auto ptr = std::make_unique<float[]>(1);
    ptr[0] = radius;
    paramSet.AddFloat("radius", std::move(ptr));

    Spectrum lightSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *WorldToObject = Translate(-pos);
    *ObjectToWorld = Translate(pos);

    std::shared_ptr<Material> light = make_shared<diffuse_light>(lightSpec.FromRGB(lightCol));
    std::shared_ptr<Shape> sphere = CreateSphereShape(ObjectToWorld, WorldToObject, false, paramSet);
    std::shared_ptr<GeometricPrimitive> light_sphere = make_shared<GeometricPrimitive>(sphere, light);

    return light_sphere;
}

std::shared_ptr<GeometricPrimitive> add_sphere(Vector3f pos, float radius, float color[3]){
    ParamSet paramSet;
    auto ptr = std::make_unique<float[]>(1);
    ptr[0] = radius;
    paramSet.AddFloat("radius", std::move(ptr));

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *WorldToObject = Translate(-pos);
    *ObjectToWorld = Translate(pos);

    std::shared_ptr<Material> mat = make_shared<lambertian>(rgbSpec.FromRGB(color));
    std::shared_ptr<Shape> sphere = CreateSphereShape(ObjectToWorld, WorldToObject, false, paramSet);
    std::shared_ptr<GeometricPrimitive> sphere_geo = make_shared<GeometricPrimitive>(sphere, mat);

    return sphere_geo;
}

std::shared_ptr<Primitive> test_sphere() {

    std::vector<std::shared_ptr<Primitive>> objects;
    std::shared_ptr<Primitive> accel;
    ParamSet paramSet;
    float lightCol[3] = {15, 15, 15};
    auto light_sphere = add_light_sphere(Vector3f(0, 0, 0), 100.0, lightCol);
    float color[3] = {1.0, 0.0, 0.0};
    auto sphere1 = add_sphere(Vector3f(0, 0, -150), 50.0, color);

    objects.push_back(light_sphere);
    objects.push_back(sphere1);
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, paramSet);
    return bvh;
}

int main(){

    ParallelInit();
    InitProfiler();

    // Image
    auto aspect_ratio = 1.0;
    int image_width = 500;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 50;
    const int max_depth = 5;

    // World
    // hittable_list world = cornell_box();
    std::shared_ptr<Primitive> world = test_sphere();
    // size_t start = 0;
    // size_t end = world.objects.size() -1 ;
    // auto world_bvh = bvh_node(world.objects, start, end);

    // Camera

    Point3f lookfrom(0, 0, -800);
    Point3f lookat(0, 0, 0);
    Vector3f vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    Spectrum background(0.0);

    camera cam(lookfrom, lookat, vup, 40.0, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    
    // Render
    std::cout<<"P3/n"<< image_width << ' ' << image_height << "\n255\n";
    
    for(int j = image_height-1; j>=0; j--){
        std::cerr << "\rScanlines remaining: "<< j <<' '<<std::flush;
        for(int i = 0; i<image_width; i++){
            Spectrum pixel_color(0.0);
            for (int s = 0; s < samples_per_pixel; ++s){
                auto u = (i + pbrt::random_float()) / (image_width - 1);
                auto v = (j + pbrt::random_float()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr<<"\nDone.\n";
}