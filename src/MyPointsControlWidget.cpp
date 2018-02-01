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

#define ADD_VALIDATOR_POINT(index, validator)                       \
    do {                                                            \
        WidgetUi->point##index##XLineEdit->setValidator(validator); \
        WidgetUi->point##index##YLineEdit->setValidator(validator); \
        WidgetUi->point##index##ZLineEdit->setValidator(validator); \
    } while (0)
#define ADD_VALIDATOR_POINTS(validator)    \
    do {                                   \
        ADD_VALIDATOR_POINT(0, validator); \
        ADD_VALIDATOR_POINT(1, validator); \
        ADD_VALIDATOR_POINT(2, validator); \
        ADD_VALIDATOR_POINT(3, validator); \
    } while (0)

MyPointsControlWidget::MyPointsControlWidget(const char* curveName,
                                             QWidget* parent)
    : MyPointsControlWidget(parent) {
    WidgetUi->curveNameLabel->setText(curveName);

    constexpr auto regexStr = "0\\.\\d{1,4}";
    const auto regexp = QRegExp(regexStr);
    auto validator = new QRegExpValidator(regexp);

    ADD_VALIDATOR_POINTS(validator);
}

#undef ADD_VALIDATOR_POINTS
#undef ADD_VALIDATOR_POINT

MyPointsControlWidget::~MyPointsControlWidget() {
    delete WidgetUi;
}

#define SET_POINT_COORD(index, COORD, value)          \
    WidgetUi->point##index##COORD##LineEdit->setText( \
        QString().setNum(value, 'g', 4))
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

const PointArray& MyPointsControlWidget::GetPoints() {
    ApplyPoints();
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
