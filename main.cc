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
#include "lights/distant.h"
#include "samplers/random.h"
#include "shapes/sphere.h"
#include "shapes/plymesh.h"
#include "shapes/disk.h"
#include "shapes/heightfield.h"

#include "materials/matte.h"
#include "materials/glass.h"
#include "materials/disney.h"
#include "integrators/directlighting.h"
#include "integrators/path.h"
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

std::shared_ptr<Material> add_glass_mat(){
    ParamSet matParams;
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();
    
    auto roughness = std::make_unique<float[]>(1);
    roughness[0] = 0.0;
    matParams.AddFloat("uroughness", std::move(roughness), 1);
    matParams.AddFloat("vroughness", std::move(roughness), 1);

    auto eta = std::make_unique<float[]>(1);
    eta[0] = 1.5;
    matParams.AddFloat("eta", std::move(eta), 1);

    TextureParams texParams(matParams, matParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateGlassMaterial(texParams);
    return std::shared_ptr<Material>(mat);
}

std::shared_ptr<Material> add_matte_mat(Vector3f color){
    ParamSet matParams;
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();

    std::unique_ptr<float[]> col(new float[3]);
    for (int j = 0; j < 3; ++j) col[j] = color[j];
    matParams.AddRGBSpectrum("Kd", std::move(col), 3);

    TextureParams texParams(matParams, matParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateMatteMaterial(texParams);
    return std::shared_ptr<Material>(mat);
}

std::shared_ptr<Material> add_disney_mat(Vector3f color, float metallic){
    ParamSet matParams;
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();

    std::unique_ptr<float[]> col(new float[3]);
    for (int j = 0; j < 3; ++j) col[j] = color[j];
    matParams.AddRGBSpectrum("color", std::move(col), 3);

    std::unique_ptr<float[]> met(new float[1]);
    met[0] = metallic;
    matParams.AddFloat("metallic", std::move(met), 1);

    TextureParams texParams(matParams, matParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateDisneyMaterial(texParams);
    return std::shared_ptr<Material>(mat);
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
    std::shared_ptr<Material> mat = add_matte_mat(Vector3f(1.0,1.0,1.0));
    std::shared_ptr<AreaLight> area; 

    return std::make_shared<GeometricPrimitive>(disk, mat, area);
}

std::vector<std::shared_ptr<Shape>> add_plane_shape(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale){
    ParamSet planeParams;
    Transform *plane2World = new Transform;
    Transform *world2plane = new Transform;
    auto nx = std::make_unique<int[]>(1);
    nx[0] = 2;
    planeParams.AddInt("nu", std::move(nx), 1);
    auto ny = std::make_unique<int[]>(1);
    ny[0] = 2;
    planeParams.AddInt("nv", std::move(ny), 1);
    auto pz = std::make_unique<float[]>(4);
    for (int j = 0; j < 4; ++j) pz[j] = 0.0;
    planeParams.AddFloat("Pz", std::move(pz), 4);

    *plane2World = Translate(pos) * RotateX(rot.x) * RotateY(rot.y) * RotateZ(rot.z) * 
        Scale(scale.x, scale.y, scale.z);
    *world2plane = Inverse(*plane2World);
    std::vector<std::shared_ptr<Shape>> plane = CreateHeightfield(plane2World, world2plane, false, planeParams);
    return plane;
}

std::vector<std::shared_ptr<Primitive>> add_plane_prim(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale, Vector3f color,
                                                        std::shared_ptr<AreaLight> area=nullptr){
    std::vector<std::shared_ptr<Primitive>> prims;
    std::vector<std::shared_ptr<Shape>> plane = add_plane_shape(pos, rot, scale);
    
    std::shared_ptr<Material> mat = add_matte_mat(color);
    for (auto s : plane) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area));
    }
    return prims;
}

std::shared_ptr<Light> add_point_light(Vector3f pos, float intensity){
    ParamSet lightParams;
    Transform light2World;
    light2World = Translate(pos);
    std::unique_ptr<float[]> in(new float[3]);
    for (int j = 0; j < 3; ++j) in[j] = intensity;
    lightParams.AddRGBSpectrum("I", std::move(in), 3);
    auto pointLight = CreatePointLight(light2World, NULL, lightParams);
    return pointLight;
}

std::shared_ptr<Light> add_distant_light(Point3f dir, float intensity){
    ParamSet lightParams;
    Transform light2World;
    light2World = Translate(Vector3f(0,0,0));
    std::unique_ptr<float[]> in(new float[3]);
    for (int j = 0; j < 3; ++j) in[j] = intensity;
    lightParams.AddRGBSpectrum("I", std::move(in), 3);
    std::unique_ptr<Point3f[]> to(new Point3f[1]);
    to[0] = dir;
    lightParams.AddPoint3f("to", std::move(to), 1);
    auto pointLight = CreateDistantLight(light2World, lightParams);
    return pointLight;
}

std::shared_ptr<AreaLight> add_area_light(std::shared_ptr<Shape> shape, Vector3f pos, Vector3f intensity){
    Transform light2World;
    light2World = Translate(pos);
    ParamSet areaLightParams;
    std::unique_ptr<float[]> Le(new float[3]);
    for (int j = 0; j < 3; ++j) Le[j] = intensity[j];
    areaLightParams.AddRGBSpectrum("L", std::move(Le), 3);

    auto nSamples = std::make_unique<int[]>(1);
    nSamples[0] = 1;
    areaLightParams.AddInt("samples", std::move(nSamples), 1);
    auto areaLight = CreateDiffuseAreaLight(light2World, NULL, areaLightParams, shape);
    return areaLight;
}

std::shared_ptr<const Camera> add_camera(Point3f origin, Point3f lookAt, Vector3f up, float fovc, int image_width, int image_height){
    ParamSet camParams;
    Transform *camToWorld = new Transform;
    *camToWorld = Inverse(LookAt(origin, lookAt, up));
    AnimatedTransform animatedCam2World(camToWorld, 0, camToWorld, 1.0);

    auto fov = std::make_unique<float[]>(1);
    fov[0] = fovc;
    camParams.AddFloat("fov", std::move(fov), 1);

    ParamSet emptyParam;
    std::unique_ptr<Filter> boxFilter = std::unique_ptr<Filter>(CreateBoxFilter(emptyParam));
    
    ParamSet filmParam;
    auto x_res = std::make_unique<int[]>(1);
    x_res[0] = image_width;
    auto y_res = std::make_unique<int[]>(1);
    y_res[0] = image_height;
    filmParam.AddInt("xresolution", std::move(x_res), 1);
    filmParam.AddInt("yresolution", std::move(y_res), 1);
    Film *film = CreateFilm(filmParam, std::move(boxFilter));

    PerspectiveCamera *cam = CreatePerspectiveCamera(camParams, animatedCam2World, film, NULL);
    return std::shared_ptr<const Camera>(cam);
}

std::vector<std::shared_ptr<Primitive>> add_stanford_bunny(Vector3f pos, float color[3]){
    ParamSet paramSet;

    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();
    TextureParams texParams(paramSet, paramSet, 
        *floatTextures1, *spectrumTextures1);
    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/Scripts/ply/bunny3.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    //*ObjectToWorld = Translate(pos) * RotateX(90) * Scale(1000, 1000, 1000);
    *ObjectToWorld = Translate(pos) * RotateY(180) * Scale(3000, 3000, 3000);
    *WorldToObject = Inverse(*ObjectToWorld);

    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 
    auto mat = add_disney_mat(Vector3f(1.0,1.0,1.0), 1);

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area));
    }

    return prims;
}

void add_cornell_box(std::vector<std::shared_ptr<Primitive>> &objects,
                                                        std::vector<std::shared_ptr<Light>> &lights){
    Vector3f white(.73, .73, .73);
    Vector3f red(.65, .05, .05);
    Vector3f green(.12, .45, .15);

    Vector3f downPos(0, 0, 0);
    Vector3f downRot(90,0,0);
    Vector3f downScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> down = add_plane_prim(downPos, downRot, downScale, white);

    Vector3f upPos(0, 555, 0);
    Vector3f upRot(90,0,0);
    Vector3f upScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> up = add_plane_prim(upPos, upRot, upScale, white);

    Vector3f backPos(0, 0, 555);
    Vector3f backRot(0,0,0);
    Vector3f backScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> back = add_plane_prim(backPos, backRot, backScale, white);

    Vector3f leftPos(0, 0, 555);
    Vector3f leftRot(0,90,0);
    Vector3f leftScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> left = add_plane_prim(leftPos, leftRot, leftScale, red);

    Vector3f rightPos(555, 0, 555);
    Vector3f rightRot(0,90,0);
    Vector3f rightScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> right = add_plane_prim(rightPos, rightRot, rightScale, green);

    Vector3f lightPos(213, 554, 227);
    Vector3f lightRot(90,0,0);
    Vector3f lightScale(130, 105, 1);
    Vector3f lightIntensity(10.0, 10.0, 10.0);
    auto areaShape = add_plane_shape(lightPos, lightRot, lightScale);
    for(auto s : areaShape){
        auto areaLight = add_area_light(s, lightPos, lightIntensity);
        std::shared_ptr<Material> mat;
        lights.push_back(areaLight);
        objects.push_back(std::make_shared<GeometricPrimitive>(s, mat, areaLight));
    }

    objects += down + up + back + left + right;
}


int main(){

    ParallelInit();
    InitProfiler();

    // World
    std::vector<std::shared_ptr<Primitive>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // Stanford bunny
    float color[3] = {1.0, 1.0, 1.0};
    objects += add_stanford_bunny(Vector3f(265,-100,295), color);

    // Planes
    add_cornell_box(objects, lights);

    // Point light
    // auto pointLight = add_point_light(Vector3f(0, 40, 1700), 100000.0);
    //lights.push_back(pointLight);

    // Directional light
    // shared_ptr<Light> dirLight = add_distant_light(Point3f(0, -1, 0), 10);
    //lights.push_back(dirLight);

    // Create BVH
    ParamSet bvhParams;
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, bvhParams);

    Scene scene(bvh, lights);

    // Camera
    Point3f origin(278, 278, -800);
    Point3f lookAt(278, 278, 0);
    Vector3f up(0, 1, 0);
    float fov = 40.0;
    auto camera = add_camera(origin, lookAt, up, fov, 500, 500);
    
    // Sampler
    ParamSet sampParams;
    auto samplePerPixel = std::make_unique<int[]>(1);
    samplePerPixel[0] = 20;
    sampParams.AddInt("pixelsamples", std::move(samplePerPixel), 1);
    auto sampler = CreateRandomSampler(sampParams);

    // Integrator
    ParamSet integParams;
    auto maxDepth = std::make_unique<int[]>(1);
    maxDepth[0] = 5;
    integParams.AddInt("maxdepth", std::move(maxDepth), 1);
    auto integrator = CreatePathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);

    // Render
    integrator->Render(scene);
}