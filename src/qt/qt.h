#ifndef QT_H
#define QT_H

#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QLayout>
#include <QSpinBox>
#include <QStyleFactory>
#include <QRadioButton>
#include <QButtonGroup>

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfRgba.h>
#include <ImfRgbaFile.h>

#include "pbrt.h"

QSpinBox* createSpinBox(int rangeMin, int rangeMax, int initValue, QString suffix,
                        QVBoxLayout* layout);
QDoubleSpinBox* createDoubleSpinBox(double rangeMin, double rangeMax, double initValue, 
                                    QString suffix, QVBoxLayout* layout);
QButtonGroup* createIntegratorGroup(QVBoxLayout* layout);
QImage displayEXR(std::string dir);

#endif