#include "pbrt.h"
#include "spectrum.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include "color.h"

#include "paramset.h"
#include "primitive.h"
#include "camera.h"
#include "cameras/perspective.h"
#include "bvh.h"
#include "shapes/sphere.h"
#include "shapes/plymesh.h"
#include "scene.h"
#include "filters/box.h"

#include "stats.h"
#include "parallel.h"

#include <iostream>
#include <map>

using namespace pbrt;

Spectrum ray_color(
    const Ray& r,
    const Spectrum& background, 
    Scene& scene,
    int depth
){
    SurfaceInteraction si;

    if (depth <= 0)
        return Spectrum(0.0);
    if (!scene.Intersect(r, &si)) 
        return background;
    scatter_record srec;
    Spectrum emitted = si.primitive->GetMaterial()->emitted(r, si, si.uv[0], si.uv[1], si.p);
    if(!si.primitive->GetMaterial()->scatter(r, si, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation
            * ray_color(srec.specular_ray, background, scene, depth-1);
    }
    
    // auto light_ptr = make_shared<hittable_pdf>(lights, si.p);
    // mixture_pdf p(light_ptr, srec.pdf_ptr);

    Ray scattered = Ray(si.p, Random_in_unit_sphere());
    // auto pdf_val = p.value(scattered.direction());

    return emitted + srec.attenuation * si.primitive->GetMaterial()->scatter(r, si, srec)
                            * ray_color(scattered, background, scene, depth-1);
}

std::shared_ptr<GeometricPrimitive> add_light_sphere(Vector3f pos, float radius, float lightCol[3]){
    ParamSet paramSet;
    auto ptr = std::make_unique<float[]>(1);
    ptr[0] = radius;
    paramSet.AddFloat("radius", std::move(ptr));

    Spectrum lightSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *ObjectToWorld = Translate(pos);
    *WorldToObject = Inverse(*ObjectToWorld);

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

std::vector<std::shared_ptr<Primitive>> add_stanford_bunny(Vector3f pos, float color[3]){
    ParamSet paramSet;
    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("ply/bunny.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *ObjectToWorld = Translate(pos) * Scale(1000, 1000, 1000);
    *WorldToObject = Inverse(*ObjectToWorld);

    std::shared_ptr<Material> mat = make_shared<lambertian>(rgbSpec.FromRGB(color));
    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat));
    }

    return prims;
}

std::shared_ptr<Primitive> test_sphere() {

    std::vector<std::shared_ptr<Primitive>> objects;
    std::shared_ptr<Primitive> accel;
    ParamSet paramSet;
    float lightCol[3] = {15, 15, 15};
    auto light_sphere = add_light_sphere(Vector3f(0, 0, 0), 100.0, lightCol);
    float color[3] = {1.0, 0.0, 0.0};
    auto sphere1 = add_sphere(Vector3f(0, 0, -150), 50.0, color);
    auto bunny = add_stanford_bunny(Vector3f(0, -150, -300), color);
    objects.push_back(light_sphere);
    //objects.push_back(sphere1);
    objects.insert(objects.end(), bunny.begin(), bunny.end());
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
    int samples_per_pixel = 30;
    const int max_depth = 5;

    // World
    // hittable_list world = cornell_box();
    std::shared_ptr<Primitive> objects = test_sphere();
    std::vector<std::shared_ptr<Light>> lights;
    Scene scene(objects, lights);

    // Camera

    ParamSet emptyParam;
    Point3f lookfrom(0, 0, -800);
    Point3f lookat(0, 0, 0);
    Vector3f vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    Spectrum background(0.0);

    Transform *camToWorld = new Transform;
    *camToWorld = Translate(Vector3f(0, 0, -800));

    AnimatedTransform animatedCam2World(camToWorld, 0, camToWorld, 1.0);
    std::unique_ptr<Filter> boxFilter = std::unique_ptr<Filter>(CreateBoxFilter(emptyParam));
    
    ParamSet filmParam;
    auto x_res = std::make_unique<int[]>(1);
    x_res[0] = image_width;
    auto y_res = std::make_unique<int[]>(1);
    y_res[0] = image_height;
    filmParam.AddInt("xresolution", std::move(x_res), 1);
    filmParam.AddInt("yresolution", std::move(y_res), 1);
    Film *film = CreateFilm(filmParam, std::move(boxFilter));

    PerspectiveCamera *cam = CreatePerspectiveCamera(emptyParam, animatedCam2World, film);
    std::unique_ptr<Sampler> tileSampler;
    // Render
    std::cout<<"P3/n"<< image_width << ' ' << image_height << "\n255\n";
    
    for(int j = image_height-1; j>=0; j--){
        std::cerr << "\rScanlines remaining: "<< j <<' '<<std::flush;
        for(int i = 0; i<image_width; i++){
            Spectrum pixel_color(0.0);
            for (int s = 0; s < samples_per_pixel; ++s){
                // auto u = (i + pbrt::random_float()) / (image_width - 1);
                // auto v = (j + pbrt::random_float()) / (image_height - 1);
                CameraSample cs;
                cs.pFilm = Point2f(i, j) + Point2f(pbrt::random_float(), pbrt::random_float());
                cs.time = pbrt::random_float();
                cs.pLens = Point2f(pbrt::random_float(),pbrt::random_float());
                Ray r;
                cam->GenerateRay(cs, &r);
                pixel_color += ray_color(r, background, scene, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr<<"\nDone.\n";
}