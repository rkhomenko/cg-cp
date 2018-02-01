// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_MY_POINTS_CONTROL_WIDGET_HPP_
#define CG_CP_MY_POINTS_CONTROL_WIDGET_HPP_

#include <Vertex.hpp>

#include <array>

#include <QWidget>

namespace Ui {
class MyPointsControlWidget;
}

class MyPointsControlWidget : public QWidget {
    Q_OBJECT

public:
    using PointArray = std::array<Vertex, 4>;

    explicit MyPointsControlWidget(QWidget* parent = nullptr);
    explicit MyPointsControlWidget(const char* curveName,
                                   QWidget* parent = nullptr);
    ~MyPointsControlWidget();

    void SetPoints(const PointArray& points);
    const PointArray& GetPoints();

public slots:
    void ApplyPoints();

private:
    PointArray Points;
    Ui::MyPointsControlWidget* WidgetUi;
};

#endif  // CG_CP_MY_POINTS_CONTROL_WIDGET_HPP_
