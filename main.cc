#include "pbrt.h"
#include "spectrum.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include "color.h"
#include "texture.h"

#include "paramset.h"
#include "primitive.h"
#include "camera.h"
#include "bvh.h"
#include "cameras/perspective.h"
#include "lights/point.h"
#include "lights/diffuse.h"
#include "samplers/random.h"
#include "shapes/sphere.h"
#include "shapes/plymesh.h"
#include "shapes/disk.h"

#include "materials/matte.h"
#include "integrators/directlighting.h"
#include "scene.h"
#include "filters/box.h"

#include "stats.h"
#include "parallel.h"

#include <iostream>
#include <map>

using namespace pbrt;

using FloatTextureMap = std::map<std::string, std::shared_ptr<Texture<float>>>;

using SpectrumTextureMap = std::map<std::string, std::shared_ptr<Texture<Spectrum>>>;
std::shared_ptr<SpectrumTextureMap> spectrumTextures;
bool spectrumTexturesShared = false;

std::vector<std::shared_ptr<Primitive>> add_stanford_bunny(Vector3f pos, float color[3]){
    ParamSet paramSet;

    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();
    TextureParams texParams(paramSet, paramSet, 
        *floatTextures1, *spectrumTextures1);
    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/Scripts/ply/bunny.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *ObjectToWorld = Translate(pos) * RotateX(90) * Scale(1000, 1000, 1000);
    *WorldToObject = Inverse(*ObjectToWorld);

    Material *mat = CreateMatteMaterial(texParams);
    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, std::shared_ptr<Material>(mat), area));
    }

    return prims;
}

std::vector<std::shared_ptr<Primitive>> stanford_bunny(Vector3f pos) {

    std::vector<std::shared_ptr<Primitive>> objects;
    std::shared_ptr<Primitive> accel;
    ParamSet paramSet;
    float color[3] = {1.0, 0.0, 0.0};
    auto bunny = add_stanford_bunny(pos, color);
    //objects.push_back(sphere1);
    objects.insert(objects.end(), bunny.begin(), bunny.end());
    return objects;
}

std::shared_ptr<Shape> sphere_shape(Vector3f pos, float radius) {
    ParamSet sphereParams;
    Transform *sphere2World = new Transform;
    Transform *world2Sphere = new Transform;
    auto r = std::make_unique<float[]>(1);
    r[0] = radius;
    sphereParams.AddFloat("radius", std::move(r), 1);
    *sphere2World = Translate(pos);
    *world2Sphere = Inverse(*sphere2World);
    auto sphere = CreateSphereShape(sphere2World, world2Sphere, false, sphereParams);
    return sphere;
}

std::shared_ptr<Primitive> basic_disk(Vector3f pos, float radius){
    ParamSet diskParams;
    Transform *disk2World = new Transform;
    Transform *world2disk = new Transform;
    auto r = std::make_unique<float[]>(1);
    r[0] = radius;
    diskParams.AddFloat("radius", std::move(r), 1);
    *disk2World = Translate(pos);
    *world2disk = Inverse(*disk2World);
    auto disk = CreateDiskShape(disk2World, world2disk, false, diskParams);
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();
    TextureParams texParams(diskParams, diskParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateMatteMaterial(texParams);
    std::shared_ptr<AreaLight> area; 

    return std::make_shared<GeometricPrimitive>(disk, std::shared_ptr<Material>(mat), area);
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
    std::vector<std::shared_ptr<Primitive>> objects = stanford_bunny(Vector3f(0, 70, -30));
    std::vector<std::shared_ptr<Light>> lights;

    // Disk
    auto disk = basic_disk(Vector3f(0, 0, 0), 10000.0);
    objects.push_back(disk);

    // Point light
    ParamSet lightParams;
    Transform light2World;
    light2World = Translate(Vector3f(0, 40, 1700));
    std::unique_ptr<float[]> intensity(new float[3]);
    for (int j = 0; j < 3; ++j) intensity[j] = 100000.0;
    lightParams.AddRGBSpectrum("I", std::move(intensity), 3);
    auto pointLight = CreatePointLight(light2World, lightParams);
    //lights.push_back(pointLight);

    // Area light
    ParamSet areaLightParams;
    std::unique_ptr<float[]> Le(new float[3]);
    for (int j = 0; j < 3; ++j) Le[j] = 20.0;
    areaLightParams.AddRGBSpectrum("L", std::move(Le), 3);
    auto areaLightShape = sphere_shape(Vector3f(0, -10, 170), 10);
    auto areaLight = CreateDiffuseAreaLight(light2World, areaLightParams, areaLightShape);
    lights.push_back(areaLight);
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();
    TextureParams texParams(areaLightParams, areaLightParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateMatteMaterial(texParams);
    auto areaPrim = std::make_shared<GeometricPrimitive>(areaLightShape, std::shared_ptr<Material>(mat), areaLight);
    objects.push_back(areaPrim);

    // Create BVH
    ParamSet bvhParams;
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, bvhParams);

    Scene scene(bvh, lights);

    // Camera

    ParamSet emptyParam;
    Transform *camToWorld = new Transform;
    Point3f origin(0, -100, 10);
    Point3f lookAt(0, 0, 0);
    Vector3f up(0, 0, 1);
    *camToWorld = Translate(Vector3f(origin)) * LookAt(origin, lookAt, up);

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
    std::shared_ptr<const Camera> camera(cam);
    // Sampler
    ParamSet empty;
    auto sampler = CreateRandomSampler(empty);

    // Integrator
    auto integrator = CreateDirectLightingIntegrator(empty, std::shared_ptr<Sampler>(sampler), camera);

    // // Render

    integrator->Render(scene);

    // std::cout<<"P3/n"<< image_width << ' ' << image_height << "\n255\n";
    
    // for(int j = image_height-1; j>=0; j--){
    //     std::cerr << "\rScanlines remaining: "<< j <<' '<<std::flush;
    //     for(int i = 0; i<image_width; i++){
    //         Spectrum pixel_color(0.0);
    //         for (int s = 0; s < samples_per_pixel; ++s){
    //             // auto u = (i + pbrt::random_float()) / (image_width - 1);
    //             // auto v = (j + pbrt::random_float()) / (image_height - 1);
    //             CameraSample cs;
    //             cs.pFilm = Point2f(i, j) + Point2f(pbrt::random_float(), pbrt::random_float());
    //             cs.time = pbrt::random_float();
    //             cs.pLens = Point2f(pbrt::random_float(),pbrt::random_float());
    //             Ray r;
    //             cam->GenerateRay(cs, &r);
    //             pixel_color += ray_color(r, background, scene, max_depth);
    //         }
    //         write_color(std::cout, pixel_color, samples_per_pixel);
    //     }
    // }
    // std::cerr<<"\nDone.\n";
}