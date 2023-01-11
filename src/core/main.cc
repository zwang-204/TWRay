#include "twray.h"

using namespace pbrt;

struct Parameters{
    int width = 500;
    int height = 500;
    int samplePerPixel = 4;
    int maxDepth = 5;
};

void Render(Parameters param){

    // World
    std::vector<std::shared_ptr<Primitive>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // Medium
    Medium *medium;
    Vector3f sigma_a(0.05, 0.05, 0.05);
    Vector3f sigma_s(0.1, 0.1, 0.1);
    medium = add_medium("", sigma_a, sigma_s, 0.0, 1);
    MediumInterface mi;

    // Stanford bunny 265,-70,295
    // float color[3] = {1.0, 1.0, 1.0};
    // objects += add_stanford_bunny(Vector3f(265,-70,295), color, mi);

    // Stanford dragon 265,-70,295
    // float color[3] = {1.0, 1.0, 1.0};
    // objects += add_stanford_dragon(Vector3f(0., -0., -0.5), color, mi);

    // add_cornell_box(objects, lights, 20.0, mi);
    // add_sample_scene(objects, lights, 2, mi);
    // add_caustics_scene(objects, lights, 0.3, mi);
    add_wine_glass_scene(objects, lights, 1, mi);
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
    Point3f origin(7.3589, -6.9258, 4.9583);
    Point3f lookAt(2.0204, -1.8232, 1);
    Vector3f up(0, 0, 1);
    float fov = 23;

    auto camera = add_camera(origin, lookAt, up, fov, param.width, param.height, mi, "twray.png");
    
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
    iterations[0] = 32;
    integParams.AddInt("numiterations", std::move(iterations), 1);

    auto radius = std::make_unique<float[]>(1);
    radius[0] = 0.025;
    integParams.AddFloat("radius", std::move(radius), 1);

    auto integrator = CreatePathIntegrator(integParams, std::shared_ptr<Sampler>(sampler), camera);
    // auto integrator = CreateSPPMIntegrator(integParams, camera);
    // Render
    integrator->Render(scene);
}

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    ParallelInit();
    InitProfiler();
    SetSearchDirectory("/home/ririka/PBR/TWRay/");

    Parameters param;
    param.samplePerPixel = 4;

    QLabel label;
    std::string file = AbsolutePath(ResolveFilename("build/twray.png"));
    QString dir = QString::fromStdString(file);
    QPixmap pixmap(dir);
    label.setPixmap(pixmap);
    QVBoxLayout *buttonSpinboxLayout = new QVBoxLayout;

    // Create a QSpinBox for the pixel samples
    QSpinBox *width = createSpinBox(1, 2000, 500, " Width", buttonSpinboxLayout);
    QSpinBox *height = createSpinBox(1, 2000, 500, " Height", buttonSpinboxLayout);
    QSpinBox *spp = createSpinBox(1, 1024, 4, " SPP", buttonSpinboxLayout);
    QSpinBox *depth = createSpinBox(1, 50, 5, " Depth", buttonSpinboxLayout);

    std::vector<QSpinBox*> spinBoxes = {width, height, spp, depth};

    QPushButton *renderButton = new QPushButton("Render");
    renderButton->setFixedSize(200,50);
    renderButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QObject::connect(renderButton, &QPushButton::clicked, [spinBoxes, dir, &param, &label](){ 
        param.width = spinBoxes[0]->value();
        param.height = spinBoxes[1]->value();
        param.samplePerPixel = spinBoxes[2]->value();
        param.maxDepth = spinBoxes[3]->value();
        Render(param); 
        QPixmap newPixmap(dir);
        label.setPixmap(newPixmap);
        label.update();
    });
    buttonSpinboxLayout->addWidget(renderButton);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(&label);
    layout->addLayout(buttonSpinboxLayout);

    QWidget window;
    window.setStyle(QStyleFactory::create("Fusion"));
    window.setLayout(layout);
    window.show();

    return app.exec();
}