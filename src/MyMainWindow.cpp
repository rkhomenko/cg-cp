// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyControlWidget.hpp>
#include <MyMainWindow.hpp>
#include <MyOpenGLWidget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QSurfaceFormat>
#include <QTabWidget>
#include <QVBoxLayout>

MyMainWindow::MyMainWindow(QWidget* parent) : QMainWindow(parent) {
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);

    OpenGLWidget = new MyOpenGLWidget(1.1f, 1.5f, 0.2f, 20, 60);
    OpenGLWidget->setFormat(format);

    setCentralWidget(CreateCentralWidget());
}

QWidget* MyMainWindow::CreateCentralWidget() {
    const auto fixedSizePolicy =
        QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto widget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto controlWidget = new MyControlWidget;
    auto toolLayout = new QHBoxLayout;
    auto label = new QLabel(VARIANT_DESCRIPTION);

    label->setSizePolicy(fixedSizePolicy);
    toolLayout->addWidget(controlWidget);
    toolLayout->addWidget(label);

    // set connection for redraw on scale changed
    connect(controlWidget, &MyControlWidget::ScaleUpSignal, OpenGLWidget,
            &MyOpenGLWidget::ScaleUpSlot);
    connect(controlWidget, &MyControlWidget::ScaleDownSignal, OpenGLWidget,
            &MyOpenGLWidget::ScaleDownSlot);

    // set connection for redraw on angle changed
    connect(controlWidget, &MyControlWidget::OXAngleChangedSignal, OpenGLWidget,
            &MyOpenGLWidget::OXAngleChangedSlot);
    connect(controlWidget, &MyControlWidget::OYAngleChangedSignal, OpenGLWidget,
            &MyOpenGLWidget::OYAngleChangedSlot);
    connect(controlWidget, &MyControlWidget::OZAngleChangedSignal, OpenGLWidget,
            &MyOpenGLWidget::OZAngleChangedSlot);

    // set connection for redraw on lighting params changed
    connect(controlWidget, &MyControlWidget::AmbientChangedSignal, OpenGLWidget,
            &MyOpenGLWidget::AmbientChangedSlot);
    connect(controlWidget, &MyControlWidget::SpecularChangedSignal,
            OpenGLWidget, &MyOpenGLWidget::SpecularChangedSlot);
    connect(controlWidget, &MyControlWidget::DiffuseChangedSignal, OpenGLWidget,
            &MyOpenGLWidget::DiffuseChangedSlot);

    // set connection for redraw on vertex or surface count changed
    connect(controlWidget, &MyControlWidget::VertexCountChangedSignal,
            OpenGLWidget, &MyOpenGLWidget::VertexCountChangedSlot);
    connect(controlWidget, &MyControlWidget::SurfaceCountChangedSignal,
            OpenGLWidget, &MyOpenGLWidget::SurfaceCountChangedSlot);

    mainLayout->addLayout(toolLayout);
    mainLayout->addWidget(OpenGLWidget);
    widget->setLayout(mainLayout);

    return widget;
}
