// Computer graphic curse project
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

    auto connectSlider = [this](auto&& slider, auto&& signal) {
        connect(slider, &QSlider::valueChanged, this,
                [signal, this](int value) {
                    emit std::invoke(signal, this, value);
                });
    };

    // surface drawing params connection
    connectSlider(WidgetUi->linesSlider,
                  &MyControlWidget::LineCountChangedSignal);
}

MyControlWidget::~MyControlWidget() {
    delete WidgetUi;
}
