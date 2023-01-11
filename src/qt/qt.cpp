#include "qt.h"

QSpinBox* createSpinBox(int rangeMin, int rangeMax, int initValue, QString suffix,
                        QVBoxLayout* layout){
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setRange(rangeMin, rangeMax);
    spinBox->setValue(initValue);
    spinBox->setSuffix(suffix);
    layout->addWidget(spinBox);
    return spinBox;
}