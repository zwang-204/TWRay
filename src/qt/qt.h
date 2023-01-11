#ifndef QT_H
#define QT_H

#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QLayout>
#include <QSpinBox>
#include <QStyleFactory>

QSpinBox* createSpinBox(int rangeMin, int rangeMax, int initValue, QString suffix,
                        QVBoxLayout* layout);

#endif