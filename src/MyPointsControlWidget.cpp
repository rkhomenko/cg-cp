// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyPointsControlWidget.hpp>

#include <ui_MyPointsControlWidget.h>

#include <QLabel>
#include <QLineEdit>
#include <QString>

using PointArray = MyPointsControlWidget::PointArray;

MyPointsControlWidget::MyPointsControlWidget(QWidget* parent)
    : QWidget(parent), WidgetUi(new Ui::MyPointsControlWidget) {
    WidgetUi->setupUi(this);
}

MyPointsControlWidget::MyPointsControlWidget(const char* curveName,
                                             QWidget* parent)
    : MyPointsControlWidget(parent) {
    WidgetUi->curveNameLabel->setText(curveName);
}

MyPointsControlWidget::~MyPointsControlWidget() {
    delete WidgetUi;
}

#define SET_POINT_COORD(index, COORD, value) \
    WidgetUi->point##index##COORD##LineEdit->setText(QString::number(value))
#define SET_POINT(index)                         \
    do {                                         \
        auto vec4 = Points[index].GetPosition(); \
        SET_POINT_COORD(index, X, vec4[0]);      \
        SET_POINT_COORD(index, Y, vec4[1]);      \
        SET_POINT_COORD(index, Z, vec4[2]);      \
    } while (0)

void MyPointsControlWidget::SetPoints(const PointArray& points) {
    for (auto i = 0UL; i < points.size(); i++) {
        Points[i] = points[i];
    }

    SET_POINT(0);
    SET_POINT(1);
    SET_POINT(2);
    SET_POINT(3);
}

#undef SET_POINT
#undef SET_POINT_COORD

const PointArray& MyPointsControlWidget::GetPoints() const {
    return Points;
}

#define GET_POINT_COORD(index, COORD) \
    WidgetUi->point##index##COORD##LineEdit->text().toFloat()
#define GET_POINT(index)                                             \
    Points[index] =                                                  \
        Vertex(GET_POINT_COORD(index, X), GET_POINT_COORD(index, Y), \
               GET_POINT_COORD(index, Z))

void MyPointsControlWidget::ApplyPoints() {
    GET_POINT(0);
    GET_POINT(1);
    GET_POINT(2);
    GET_POINT(3);
}

#undef GET_POINT
#undef GET_POINT_COORD
