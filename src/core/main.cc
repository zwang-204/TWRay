#include "twray.h"

using namespace pbrt;

enum class IntegratorType {
    PATHTRACER,
    VOLPATHTRACER,
    BIDIRECTIONAL,
    SPPM
};

struct Parameters{
    int width;
    int height;
    int samplePerPixel;
    int maxDepth;
    int numIterations;
    float radius;
    IntegratorType currentIntegrator;
    Parameters(int width, int height, int samplePerPixel, int maxDepth, 
            int numIterations, float radius, IntegratorType currentIntegrator)
        : width(width), height(height), samplePerPixel(samplePerPixel), maxDepth(maxDepth), 
          numIterations(numIterations), radius(radius),  currentIntegrator(currentIntegrator) {}
};

void Render(Parameters param){

    // World
    std::vector<std::shared_ptr<Primitive>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // Medium
    Medium *medium;
    Vector3f sigma_a(0.05, 0.05, 0.05);
    Vector3f sigma_s(0.1, 0.1, 0.1);
    medium = addMedium("", sigma_a, sigma_s, 0.0, 1);
    MediumInterface mi;

    // Stanford bunny 265,-70,295
    // float color[3] = {1.0, 1.0, 1.0};
    // objects += addStanfordBunny(Vector3f(265,-70,295), color, mi);

    // Stanford dragon 265,-70,295
    // float color[3] = {1.0, 1.0, 1.0};
    // objects += addStanfordDragon(Vector3f(0., -0., -0.5), color, mi);

    addCornellBox(objects, lights, 20.0, mi);
    // addSampleScene(objects, lights, 2, mi);
    // addCausticsScene(objects, lights, 0.3, mi);
    // addWineGlassScene(objects, lights, 1, mi);
    // Create BVH
    ParamSet bvhParams;
    std::shared_ptr<Primitive> bvh = CreateBVHAccelerator(objects, bvhParams);

    Scene scene(bvh, lights);

    // Camera

    // Cornell box camera params
    Point3f origin(278, 278, -800);
    Point3f lookAt(278, 278, 0);
    Vector3f up(0, 1, 0);
    float fov = 40.0;

    // Sample scene camera params
    // Point3f origin(3.69558, -3.46243, 3.25463);
    // Point3f lookAt(3.04072, -2.85176, 2.80939);
    // Vector3f up(-0.317366, 0.312466, 0.895346);
    // float fov = 28.8415038750464;
    
    // Caustics scene camera params
    // Point3f origin(-5.5, 7, -5.5);
    // Point3f lookAt(-4.75, 2.25, 0);
    // Vector3f up(0, 1, 0);
    // float fov = 40;

    // Wine glass camera params
    // Point3f origin(7.3589, -6.9258, 4.9583);
    // Point3f lookAt(2.0204, -1.8232, 1);
    // Vector3f up(0, 0, 1);
    // float fov = 23;

    auto camera = addCamera(origin, lookAt, up, fov, param.width, param.height, mi, "twray.exr");
    
    // Sampler
    ParamSet sampParams;
    auto samplePerPixel = std::make_unique<int[]>(1);
    samplePerPixel[0] = param.samplePerPixel;
    sampParams.AddInt("pixelsamples", std::move(samplePerPixel), 1);
    auto sampler = CreateHaltonSampler(sampParams, camera->film->GetSampleBounds());

    // Integrator
    ParamSet integParams;
    auto maxDepth = std::make_unique<int[]>(1);
    maxDepth[0] = param.maxDepth;
    integParams.AddInt("maxdepth", std::move(maxDepth), 1);

    auto iterations = std::make_unique<int[]>(1);
    iterations[0] = param.numIterations;
    integParams.AddInt("numiterations", std::move(iterations), 1);

    auto radius = std::make_unique<float[]>(1);
    radius[0] = param.radius;
    integParams.AddFloat("radius", std::move(radius), 1);

    if(param.currentIntegrator == IntegratorType::PATHTRACER) {
        auto integrator = CreatePathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);
        integrator->Render(scene);
    } else if(param.currentIntegrator == IntegratorType::VOLPATHTRACER) {
        auto integrator = CreateVolPathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);
        integrator->Render(scene);
    } else if(param.currentIntegrator == IntegratorType::BIDIRECTIONAL) {
        auto integrator = CreateBDPTIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);
        integrator->Render(scene);
    } else if(param.currentIntegrator == IntegratorType::SPPM) {
        auto integrator = CreateSPPMIntegrator(integParams, camera);
        integrator->Render(scene);
    } else {
        auto integrator = CreatePathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);
        integrator->Render(scene);
    }
}

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    ParallelInit();
    InitProfiler();
    SetSearchDirectory("/Users/Security/TA/C++Tutorial/RayTracing/TWRay/");

    Parameters param(500, 500, 4, 5, 64, 0.025, IntegratorType::PATHTRACER);
    param.samplePerPixel = 4;

    QLabel label;
    std::string file = AbsolutePath(ResolveFilename("build/twray.exr"));
    QImage image = displayEXR(file);
    QPixmap pixmap;
    label.setPixmap(QPixmap::fromImage(image));
    QVBoxLayout *vLayout = new QVBoxLayout;

    // Create Integrator options
    QButtonGroup* integratorGroup = createIntegratorGroup(vLayout);

    // Create a QSpinBox for the pixel samples
    QSpinBox *width = createSpinBox(1, 2000, 500, " Width", vLayout);
    QSpinBox *height = createSpinBox(1, 2000, 500, " Height", vLayout);
    QSpinBox *spp = createSpinBox(1, 2000, 4, " SPP", vLayout);
    QSpinBox *depth = createSpinBox(1, 50, 5, " Depth", vLayout);
    QSpinBox *numIt = createSpinBox(1, 1000, 4, " Num Iterations (SPPM only)", vLayout);
    QDoubleSpinBox *radius = createDoubleSpinBox(0, 2., 0.025, " Radius (SPPM only)", vLayout);

    std::vector<QSpinBox*> spinBoxes = {width, height, spp, depth, numIt};
    std::vector<QDoubleSpinBox*> doubleSpinBoxes = {radius};

    QPushButton *renderButton = new QPushButton("Render");
    renderButton->setFixedSize(200,50);
    renderButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QObject::connect(renderButton, &QPushButton::clicked, [spinBoxes, doubleSpinBoxes, radius, integratorGroup, file, &param, &label](){ 
        param.width = spinBoxes[0]->value();
        param.height = spinBoxes[1]->value();
        param.samplePerPixel = spinBoxes[2]->value();
        param.maxDepth = spinBoxes[3]->value();
        param.numIterations = spinBoxes[4]->value();
        param.radius = doubleSpinBoxes[0]->value();
        int id = -integratorGroup->checkedId() - 2;
        switch (id) {
            case 0:
                param.currentIntegrator = IntegratorType::PATHTRACER;
                break;
            case 1:
                param.currentIntegrator = IntegratorType::VOLPATHTRACER;
                break;
            case 2:
                param.currentIntegrator = IntegratorType::BIDIRECTIONAL;
                break;
            case 3:
                param.currentIntegrator = IntegratorType::SPPM;
                break;
        }
        
        Render(param); 
        QImage newImage = displayEXR(file);
        label.setPixmap(QPixmap::fromImage(newImage));
        label.update();
    });
    vLayout->addWidget(renderButton);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(&label);
    layout->addLayout(vLayout);

    QWidget window;
    window.setStyle(QStyleFactory::create("Fusion"));
    window.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    window.setLayout(layout);
    window.show();

    return app.exec();
}