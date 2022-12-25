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
    medium = add_medium("", Vector3f(.05, .05, .05), Vector3f(.1, .1, .1), 0.0, 0.01);
    MediumInterface mi(medium);
    // Stanford bunny
    //float color[3] = {1.0, 1.0, 1.0};
    //objects += add_stanford_bunny(Vector3f(265,-70,295), color, mi);

    // Stanford dragon
    float color[3] = {1.0, 1.0, 1.0};
    objects += add_stanford_dragon(Vector3f(265,-70,295), color, mi);

    // Planes
    add_cornell_box(objects, lights, 20.0, mi);

    // Create BVH
    ParamSet bvhParams;
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, bvhParams);

    Scene scene(bvh, lights);

    // Camera
    Point3f origin(278, 278, -800);
    Point3f lookAt(278, 278, 0);
    Vector3f up(0, 1, 0);
    float fov = 40.0;
    auto camera = add_camera(origin, lookAt, up, fov, 500, 500, mi);
    
    // Sampler
    ParamSet sampParams;
    auto samplePerPixel = std::make_unique<int[]>(1);
    samplePerPixel[0] = 40;
    sampParams.AddInt("pixelsamples", std::move(samplePerPixel), 1);
    auto sampler = CreateZeroTwoSequenceSampler(sampParams);

    // Integrator
    ParamSet integParams;
    auto maxDepth = std::make_unique<int[]>(1);
    maxDepth[0] = 8;
    integParams.AddInt("maxdepth", std::move(maxDepth), 1);
    auto integrator = CreateBDPTIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);

    // Render
    integrator->Render(scene);
}