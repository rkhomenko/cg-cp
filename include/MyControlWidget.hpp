// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_MYCONTROLWIDGET_HPP_
#define CG_LAB_MYCONTROLWIDGET_HPP_

#include <QWidget>

namespace Ui {
class MyControlWidget;
}

class MyControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit MyControlWidget(QWidget* parent = nullptr);
    ~MyControlWidget();

signals:
    void ScaleUpSignal();
    void ScaleDownSignal();

    void OXAngleChangedSignal(float angle);
    void OYAngleChangedSignal(float angle);
    void OZAngleChangedSignal(float angle);

    void VertexCountChangedSignal(int vertexCount);
    void SurfaceCountChangedSignal(int surfaceCount);

    void AmbientChangedSignal(float ambientCoeff);
    void SpecularChangedSignal(float specularCoeff);
    void DiffuseChangedSignal(float diffuseCoeff);

private:
    static const float PI;
    static const float TETA_MAX;

    Ui::MyControlWidget* WidgetUi;
};

#endif  // CG_LAB_MYCONTROLWIDGET_HPP_
