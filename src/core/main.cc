#include "twray.h"

using namespace pbrt;

int main(){

    ParallelInit();
    InitProfiler();

    // World
    std::vector<std::shared_ptr<Primitive>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // Medium
    Medium *medium;
    // medium = add_medium("", Vector3f(.05, .05, .05), Vector3f(.1, .1, .1), 0.0, 0.01);
    MediumInterface mi;
    // Stanford bunny 265,-70,295
    // float color[3] = {1.0, 1.0, 1.0};
    // objects += add_stanford_bunny(Vector3f(0,0,0), color, mi);

    // Stanford dragon 265,-70,295
    float color[3] = {1.0, 1.0, 1.0};
    objects += add_stanford_dragon(Vector3f(0., -0., -0.5), color, mi);

    // Cornell box
    // add_cornell_box(objects, lights, 20.0, mi);
    add_sample_scene(objects, lights, 2.5, mi);

    // Create BVH
    ParamSet bvhParams;
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, bvhParams);

    Scene scene(bvh, lights);

    // Camera
    // Cornell box camera params
    // Point3f origin(278, 278, -800);
    // Point3f lookAt(278, 278, 0);
    // Vector3f up(0, 1, 0);
    // float fov = 40.0;

    // Sample scene camera params
    Point3f origin(3.69558, -3.46243, 3.25463);
    Point3f lookAt(3.04072, -2.85176, 2.80939);
    Vector3f up(-0.317366, 0.312466, 0.895346);
    float fov = 28.8415038750464;
    auto camera = add_camera(origin, lookAt, up, fov, 500, 500, mi);
    
    // Sampler
    ParamSet sampParams;
    auto samplePerPixel = std::make_unique<int[]>(1);
    samplePerPixel[0] = 4;
    sampParams.AddInt("pixelsamples", std::move(samplePerPixel), 1);
    auto sampler = CreateZeroTwoSequenceSampler(sampParams);

    // Integrator
    ParamSet integParams;
    auto maxDepth = std::make_unique<int[]>(1);
    maxDepth[0] = 5;
    integParams.AddInt("maxdepth", std::move(maxDepth), 1);
    auto integrator = CreateVolPathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);

    // Render
    integrator->Render(scene);
}