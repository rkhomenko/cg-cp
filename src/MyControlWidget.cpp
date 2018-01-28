// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyControlWidget.hpp>

#include <ui_MyControlWidget.h>

#include <cmath>

#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>

const float MyControlWidget::PI = 4 * std::atan(1.0f);
const float MyControlWidget::TETA_MAX = 2 * MyControlWidget::PI;

MyControlWidget::MyControlWidget(QWidget* parent)
    : QWidget(parent), WidgetUi(new Ui::MyControlWidget) {
    WidgetUi->setupUi(this);

    connect(WidgetUi->scaleUpButton, &QPushButton::clicked, this,
            [this]() { emit ScaleUpSignal(); });
    connect(WidgetUi->scaleDownButton, &QPushButton::clicked, this,
            [this]() { emit ScaleDownSignal(); });

    auto calculateAngle = [](QSlider* slider, int value) {
        const auto MIN = slider->minimum();
        const auto MAX = slider->maximum();
        return 1.0f * (value - MIN) / (MAX - MIN) * TETA_MAX;
    };

    connect(WidgetUi->oxSlider, &QSlider::valueChanged, this,
            [&, this](int value) {
                auto result = calculateAngle(WidgetUi->oxSlider, value);
                emit OXAngleChangedSignal(result);
            });

    connect(WidgetUi->oySlider, &QSlider::valueChanged, this,
            [&, this](int value) {
                auto result = calculateAngle(WidgetUi->oySlider, value);
                emit OYAngleChangedSignal(result);
            });

    connect(WidgetUi->ozSlider, &QSlider::valueChanged, this,
            [&, this](int value) {
                auto result = calculateAngle(WidgetUi->ozSlider, value);
                emit OZAngleChangedSignal(result);
            });

    constexpr auto regexStr = "0\\.\\d{1,4}";
    const auto regexp = QRegExp(regexStr);
    auto validator = new QRegExpValidator(regexp);

    for (auto&& lineEdit :
         {WidgetUi->ambientLineEdit, WidgetUi->specularLineEdit,
          WidgetUi->diffuseLineEdit}) {
        lineEdit->setValidator(validator);
    }

    auto connectLineEdit = [this](auto&& lineEdit, auto&& signal) {
        connect(lineEdit, &QLineEdit::editingFinished, this,
                [lineEdit, signal, this]() {
                    auto coeff = lineEdit->text().toFloat();
                    emit std::invoke(signal, this, coeff);
                });
    };

    // light params line edit connection
    connectLineEdit(WidgetUi->ambientLineEdit,
                    &MyControlWidget::AmbientChangedSignal);
    connectLineEdit(WidgetUi->specularLineEdit,
                    &MyControlWidget::SpecularChangedSignal);
    connectLineEdit(WidgetUi->diffuseLineEdit,
                    &MyControlWidget::DiffuseChangedSignal);

    auto connectSlider = [this](auto&& slider, auto&& signal) {
        connect(slider, &QSlider::valueChanged, this,
                [signal, this](int value) {
                    emit std::invoke(signal, this, value);
                });
    };

    // ellipsoid surface and vertex count params connection
    connectSlider(WidgetUi->vetexSlider,
                  &MyControlWidget::VertexCountChangedSignal);
    connectSlider(WidgetUi->surfaceSlider,
                  &MyControlWidget::SurfaceCountChangedSignal);
}

MyControlWidget::~MyControlWidget() {
    delete WidgetUi;
}
