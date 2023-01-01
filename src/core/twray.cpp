#include "twray.h"

namespace pbrt{

using FloatTextureMap = std::map<std::string, std::shared_ptr<Texture<float>>>;

using SpectrumTextureMap = std::map<std::string, std::shared_ptr<Texture<Spectrum>>>;
std::shared_ptr<SpectrumTextureMap> spectrumTextures;
bool spectrumTexturesShared = false;

Medium* MakeMedium(const ParamSet &paramSet) {
    float sig_a_rgb[3] = {.0011f, .0024f, .014f},
          sig_s_rgb[3] = {2.55f, 3.21f, 3.77f};
    Spectrum sig_a = Spectrum::FromRGB(sig_a_rgb),
             sig_s = Spectrum::FromRGB(sig_s_rgb);
    std::string preset = paramSet.FindOneString("preset", "");
    bool found = GetMediumScatteringProperties(preset, &sig_a, &sig_s);
    if (preset != "" && !found)
        Warning("Material preset \"%s\" not found.  Using defaults.",
                preset.c_str());
    float scale = paramSet.FindOneFloat("scale", 1.f);
    float g = paramSet.FindOneFloat("g", 0.0f);
    sig_a = paramSet.FindOneSpectrum("sigma_a", sig_a) * scale;
    sig_s = paramSet.FindOneSpectrum("sigma_s", sig_s) * scale;
    Medium *m = NULL;
    m = new HomogeneousMedium(sig_a, sig_s, g);
    return m;
}

Medium* add_medium(std::string name, Vector3f sigma_a=Vector3f(.0011f, .0024f, .014f), 
                Vector3f sigma_s=Vector3f(2.55f, 3.21f, 3.77f), float g=0.0f, 
                float scale=1.0f){
    ParamSet mediumParam;
    if(name != ""){
        std::unique_ptr<std::string[]> n(new std::string[1]);
        n[0] = name;
        mediumParam.AddString("preset", std::move(n), 1);
        return MakeMedium(mediumParam);
    }

    std::unique_ptr<float[]> sig_a(new float[3]);
    for (int j = 0; j < 3; ++j) sig_a[j] = sigma_a[j];
    mediumParam.AddRGBSpectrum("sigma_a", std::move(sig_a), 3);

    std::unique_ptr<float[]> sig_s(new float[3]);
    for (int j = 0; j < 3; ++j) sig_s[j] = sigma_s[j];
    mediumParam.AddRGBSpectrum("sigma_s", std::move(sig_s), 3);

    std::unique_ptr<float[]> _g(new float[1]);
    _g[0] = g;
    mediumParam.AddFloat("g", std::move(_g), 1);

    std::unique_ptr<float[]> s(new float[1]);
    s[0] = scale;
    mediumParam.AddFloat("scale", std::move(s), 1);

    return MakeMedium(mediumParam);
}

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

std::shared_ptr<Material> add_subsurface_mat(Vector3f color, std::string name, float scale, float roughness){
    ParamSet matParams;
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();

    std::unique_ptr<float[]> col(new float[3]);
    for (int j = 0; j < 3; ++j) col[j] = color[j];
    matParams.AddRGBSpectrum("Kr", std::move(col), 3);

    std::unique_ptr<float[]> s(new float[1]);
    s[0] = scale;
    matParams.AddFloat("scale", std::move(s), 1);

    std::unique_ptr<float[]> rough(new float[1]);
    rough[0] = roughness;
    matParams.AddFloat("uroughness", std::move(rough), 1);
    matParams.AddFloat("vroughness", std::move(rough), 1);

    std::unique_ptr<std::string[]> n(new std::string[1]);
    n[0] = name;
    matParams.AddString("name", std::move(n), 1);

    TextureParams texParams(matParams, matParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateSubsurfaceMaterial(texParams);
    return std::shared_ptr<Material>(mat);
}

std::shared_ptr<Material> add_uber_mat(Vector3f Kd, Vector3f Ks, float roughness, float index){
    ParamSet matParams;
    auto floatTextures1 = std::make_shared<FloatTextureMap>();
    auto spectrumTextures1 = std::make_shared<SpectrumTextureMap>();

    std::unique_ptr<float[]> kd(new float[3]);
    for (int j = 0; j < 3; ++j) kd[j] = Kd[j];
    matParams.AddRGBSpectrum("Kd", std::move(kd), 3);

    std::unique_ptr<float[]> ks(new float[3]);
    for (int j = 0; j < 3; ++j) ks[j] = Ks[j];
    matParams.AddRGBSpectrum("Ks", std::move(ks), 3);

    std::unique_ptr<float[]> r(new float[1]);
    r[0] = roughness;
    matParams.AddFloat("roughness", std::move(r), 1);

    std::unique_ptr<float[]> in(new float[1]);
    in[0] = index;
    matParams.AddFloat("index", std::move(in), 1);

    TextureParams texParams(matParams, matParams, 
        *floatTextures1, *spectrumTextures1);
    Material *mat = CreateUberMaterial(texParams);
    return std::shared_ptr<Material>(mat);
}

std::shared_ptr<Shape> add_sphere_shape(Vector3f pos, float radius) {
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

std::shared_ptr<Primitive> add_basic_disk(Vector3f pos, float radius, MediumInterface mi){
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

    return std::make_shared<GeometricPrimitive>(disk, mat, area, mi);
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
                                                        std::shared_ptr<AreaLight> area,
                                                        MediumInterface mi){
    std::vector<std::shared_ptr<Primitive>> prims;
    std::vector<std::shared_ptr<Shape>> plane = add_plane_shape(pos, rot, scale);
    
    std::shared_ptr<Material> mat = add_matte_mat(color);
    for (auto s : plane) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area, mi));
    }
    return prims;
}

std::shared_ptr<Light> add_point_light(Vector3f pos, Vector3f intensity, MediumInterface &mi){
    ParamSet lightParams;
    Transform light2World;
    light2World = Translate(pos);
    std::unique_ptr<float[]> in(new float[3]);
    for (int j = 0; j < 3; ++j) in[j] = intensity[j] * 4400;
    lightParams.AddRGBSpectrum("I", std::move(in), 3);
    auto pointLight = CreatePointLight(light2World, mi.outside, lightParams);
    return pointLight;
}

std::shared_ptr<Light> add_spot_light(Vector3f pos, Vector3f intensity, Vector3f to, float coneangle, MediumInterface &mi){
    ParamSet lightParams;
    Transform light2World;
    std::unique_ptr<float[]> in(new float[3]);
    for (int j = 0; j < 3; ++j) in[j] = intensity[j] * 100000;
    lightParams.AddRGBSpectrum("I", std::move(in), 3);

    std::unique_ptr<float[]> ang(new float[1]);
    ang[0] = coneangle;
    lightParams.AddFloat("coneangle", std::move(ang), 1);

    std::unique_ptr<Point3f[]> f(new Point3f[1]);
    for (int j = 0; j < 3; ++j) f[0][j] = pos[j];
    lightParams.AddPoint3f("from", std::move(f), 1);

    std::unique_ptr<Point3f[]> t(new Point3f[1]);
    for (int j = 0; j < 3; ++j) t[0][j] = to[j];
    lightParams.AddPoint3f("to", std::move(t), 1);
    auto spotLight = CreateSpotLight(light2World, mi.outside, lightParams);
    return spotLight;
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

std::shared_ptr<AreaLight> add_area_light(std::shared_ptr<Shape> shape, Vector3f pos, 
                                            Vector3f intensity, MediumInterface &mi){
    Transform light2World;
    light2World = Translate(pos);
    ParamSet areaLightParams;
    std::unique_ptr<float[]> Le(new float[3]);
    for (int j = 0; j < 3; ++j) Le[j] = intensity[j];
    areaLightParams.AddRGBSpectrum("L", std::move(Le), 3);

    auto nSamples = std::make_unique<int[]>(1);
    nSamples[0] = 1;
    areaLightParams.AddInt("samples", std::move(nSamples), 1);
    auto areaLight = CreateDiffuseAreaLight(light2World, mi.outside, areaLightParams, shape);
    return areaLight;
}

std::shared_ptr<Light> add_infinite_light(std::string filename, Vector3f intensity, MediumInterface &mi){
    ParamSet lightParams;
    Transform light2World;
    std::unique_ptr<float[]> in(new float[3]);
    for (int j = 0; j < 3; ++j) in[j] = intensity[j];
    lightParams.AddRGBSpectrum("L", std::move(in), 3);

    auto name = std::make_unique<std::string[]>(1);
    name[0] = filename;
    lightParams.AddString("mapname", std::move(name), 1);

    auto infiniteLight = CreateInfiniteLight(light2World, lightParams);
    return infiniteLight;
}

std::shared_ptr<const Camera> add_camera(Point3f origin, Point3f lookAt, Vector3f up, 
                                        float fovc, int image_width, int image_height, 
                                        MediumInterface mi){
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

    PerspectiveCamera *cam = CreatePerspectiveCamera(camParams, animatedCam2World, film, mi.outside);
    return std::shared_ptr<const Camera>(cam);
}

std::vector<std::shared_ptr<Primitive>> add_stanford_bunny(Vector3f pos, float color[3], MediumInterface mi){
    ParamSet paramSet;

    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/TWRay/ply/bunny_uncomp.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    //*ObjectToWorld = Translate(pos) * RotateX(90) * Scale(1000, 1000, 1000);
    *ObjectToWorld = Translate(pos) * RotateY(180) * Scale(2000, 2000, 2000);
    *WorldToObject = Inverse(*ObjectToWorld);

    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 
    // std::shared_ptr<Material> mat;
    // auto mat = add_subsurface_mat(Vector3f(1.0, 1.0, 1.0), "Apple", 1.0, 0.05);
    auto mat = add_glass_mat();
    // mi.inside = add_medium("", Vector3f(0.06, .06, .06), Vector3f(.2, .2, .2), 0.7, 0.2);
    mi.inside = add_medium("Apple");
    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area, mi));
    }

    return prims;
}

std::vector<std::shared_ptr<Primitive>> add_stanford_dragon(Vector3f pos, float color[3], MediumInterface mi){
    ParamSet paramSet;

    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/TWRay/ply/dragon_uncomp.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    *ObjectToWorld = Translate(pos) * RotateZ(40) * RotateX(90) * Scale(10, 10, 10);
    //*ObjectToWorld = Translate(pos) * RotateY(180) * Scale(2000, 2000, 2000);
    *WorldToObject = Inverse(*ObjectToWorld);

    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 
    auto mat = add_subsurface_mat(Vector3f(1.0, 1.0, 1.0), "Marble", 50.0, 0.0);
    // auto mat = add_glass_mat();

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area, mi));
    }

    return prims;
}

std::vector<std::shared_ptr<Primitive>> add_glass_bottle(Vector3f pos, float color[3], MediumInterface mi){
    ParamSet paramSet;

    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/TWRay/ply/glass.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    // *ObjectToWorld = Translate(pos) * RotateX(-90) * Scale(0.3, 0.3, 0.3) * Translate(Vector3f(-1, -1, -1)); 
    *ObjectToWorld = Translate(pos);
    *WorldToObject = Inverse(*ObjectToWorld);

    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 
    auto mat = add_glass_mat();

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area, mi));
    }

    return prims;
}

std::vector<std::shared_ptr<Primitive>> add_caustics_plane(MediumInterface mi){
    ParamSet paramSet;

    std::vector<std::shared_ptr<Primitive>> prims;

    auto filename = std::make_unique<std::string[]>(1);
    filename[0] = std::string("/Users/Security/TA/C++Tutorial/RayTracing/TWRay/ply/plane.ply");
    paramSet.AddString("filename", std::move(filename), 1);

    std::map<std::string, std::shared_ptr<Texture<float>>> *floatTextures;

    Spectrum rgbSpec(0.0);

    Transform *ObjectToWorld = new Transform;
    Transform *WorldToObject = new Transform;

    std::vector<std::shared_ptr<Shape>> shapes = CreatePLYMesh(ObjectToWorld, WorldToObject, false, paramSet, floatTextures);
    std::shared_ptr<AreaLight> area; 
    Vector3f kd(0.6399999857, 0.6399999857, 0.6399999857);
    Vector3f ks(0.1000000015, 0.1000000015, 0.1000000015);
    float roughness = 0.01;
    float index = 1.0;
    auto mat = add_uber_mat(kd, ks, roughness, index);

    for (auto s : shapes) {
        prims.push_back(
            std::make_shared<GeometricPrimitive>(s, mat, area, mi));
    }

    return prims;
}

void add_cornell_box(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi){
    Vector3f white(.73, .73, .73);
    Vector3f red(.65, .05, .05);
    Vector3f green(.12, .45, .15);

    Vector3f downPos(0, 0, 0);
    Vector3f downRot(90,0,0);
    Vector3f downScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> down = add_plane_prim(downPos, downRot, downScale, white, nullptr, mi);

    Vector3f upPos(0, 555, 0);
    Vector3f upRot(90,0,0);
    Vector3f upScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> up = add_plane_prim(upPos, upRot, upScale, white, nullptr, mi);

    Vector3f backPos(0, 0, 555);
    Vector3f backRot(0,0,0);
    Vector3f backScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> back = add_plane_prim(backPos, backRot, backScale, white, nullptr, mi);

    Vector3f leftPos(0, 0, 555);
    Vector3f leftRot(0,90,0);
    Vector3f leftScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> left = add_plane_prim(leftPos, leftRot, leftScale, red, nullptr, mi);

    Vector3f rightPos(555, 0, 555);
    Vector3f rightRot(0,90,0);
    Vector3f rightScale(555, 555, 1);
    std::vector<std::shared_ptr<Primitive>> right = add_plane_prim(rightPos, rightRot, rightScale, green, nullptr, mi);

    // std::string mapName = "/Users/Security/TA/C++Tutorial/RayTracing/TWRay/textures/envmap.exr";
    // auto infiniteLight = add_infinite_light(mapName, Vector3f(intensity, intensity, intensity), mi);
    // lights.push_back(infiniteLight);

    Vector3f lightPos(213, 554, 227);
    Vector3f lightRot(90,0,0);
    Vector3f lightScale(130, 105, 1);
    Vector3f lightIntensity(intensity, intensity, intensity);
    // auto areaShape = add_plane_shape(lightPos, lightRot, lightScale);
    // for(auto s : areaShape){
    //     auto areaLight = add_area_light(s, lightPos, lightIntensity, mi);
    //     std::shared_ptr<Material> mat;
    //     lights.push_back(areaLight);
    //     objects.push_back(std::make_shared<GeometricPrimitive>(s, mat, areaLight, mi));
    // }

    //auto light = add_spot_light(Vector3f(278, 554, 278), lightIntensity, Vector3f(278, 0, 278), 35, mi);
    //lights.push_back(light);

    auto light = add_point_light(Vector3f(278, 554, 278), lightIntensity, mi);
    lights.push_back(light);

    std::string mapName = "/Users/Security/TA/C++Tutorial/RayTracing/TWRay/textures/envmap.exr";
    auto inflight = add_infinite_light(mapName, Vector3f(5, 5, 5), mi);
    lights.push_back(inflight);

    objects += down + up + back + left + right;
}

void add_sample_scene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi){
    Vector3f planePos(-1000,-1000,0);
    Vector3f planeRot(0,0,0);
    Vector3f planeScale(2000,2000,2000);
    Vector3f planeColor(0.7, 0.7, 0.7);
    auto plane = add_plane_prim(planePos, planeRot, planeScale, planeColor, nullptr, mi);
    objects += plane;

    float color[3] = {1.0, 1.0, 1.0};
    objects += add_stanford_dragon(Vector3f(0., -0., -0.5), color, mi);

    std::string mapName = "/Users/Security/TA/C++Tutorial/RayTracing/TWRay/textures/envmap.exr";
    auto light = add_infinite_light(mapName, Vector3f(intensity, intensity, intensity), mi);
    lights.push_back(light);

}

void add_caustics_scene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi){
    // add_cornell_box(objects, lights, intensity, mi);
    auto plane = add_caustics_plane(mi);
    objects += plane;

    std::string mapName = "/Users/Security/TA/C++Tutorial/RayTracing/TWRay/textures/envmap.exr";
    auto light = add_infinite_light(mapName, Vector3f(intensity, intensity, intensity), mi);
    lights.push_back(light);

    Vector3f spotPos(0, 5, 9);
    Vector3f spotTo(-5, 2.75, 0);
    auto spotLight = add_spot_light(spotPos, Vector3f(0.002, 0.002, 0.002), spotTo, 30, mi);
    lights.push_back(spotLight);

    Vector3f glassPos(0, 0, 0);
    float color[3] = {1.0, 1.0, 1.0};
    objects += add_glass_bottle(glassPos, color, mi);
}


} // namespace pbrt