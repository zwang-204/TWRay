#include "qt.h"

float srgb_encode(float val)
{
    if(val>1.0f)
        val = 1.0f;
    if (val <= 0.0031308f)
        return val * 12.92f;
    else
        return 1.055f * pow(val, 1.0f / 2.4f) - 0.055f;
}

QSpinBox* createSpinBox(int rangeMin, int rangeMax, int initValue, QString suffix,
                        QVBoxLayout* layout){
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setRange(rangeMin, rangeMax);
    spinBox->setValue(initValue);
    spinBox->setSuffix(suffix);
    layout->addWidget(spinBox);
    return spinBox;
}

QDoubleSpinBox* createDoubleSpinBox(double rangeMin, double rangeMax, double initValue, 
                                    QString suffix, QVBoxLayout* layout) {
    QDoubleSpinBox *spinBox = new QDoubleSpinBox;
    spinBox->setRange(rangeMin, rangeMax);
    spinBox->setValue(initValue);
    spinBox->setSuffix(suffix);
    layout->addWidget(spinBox);
    return spinBox;
}

QButtonGroup* createIntegratorGroup(QVBoxLayout* layout) {
    QRadioButton* radioPT = new QRadioButton("PT");
    QRadioButton* radioVPT = new QRadioButton("VPT");
    QRadioButton* radioBDPT = new QRadioButton("BDPT");
    QRadioButton* radioSPPM = new QRadioButton("SPPM");
    layout->addWidget(radioPT);
    layout->addWidget(radioVPT);
    layout->addWidget(radioBDPT);
     layout->addWidget(radioSPPM);

    QButtonGroup* group = new QButtonGroup();
    group->addButton(radioPT);
    group->addButton(radioVPT);
    group->addButton(radioBDPT);
    group->addButton(radioSPPM);

    return group;
}

QImage displayEXR(const std::string dir) {
    using namespace Imf;
    using namespace Imath;
    using namespace Iex;

    RgbaInputFile file(dir.c_str());
    Box2i dw = file.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    std::vector<Rgba> pixels(width * height);
    file.setFrameBuffer(&pixels[0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels(dw.min.y, dw.max.y);

    QImage img(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Rgba rgba = pixels[y * width + x];
            float r = srgb_encode(rgba.r);
            float g = srgb_encode(rgba.g);
            float b = srgb_encode(rgba.b);
            img.setPixel(x, y, qRgb((int)(255 * r), (int)(255 * g), (int)(255 * b)));
        }
    }
    return img;
}

