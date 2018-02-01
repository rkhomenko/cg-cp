// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyControlWidget.hpp>
#include <MyMainWindow.hpp>
#include <MyOpenGLWidget.hpp>
#include <PointsDocument.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSurfaceFormat>
#include <QTabWidget>
#include <QVBoxLayout>

MyMainWindow::MyMainWindow(QWidget* parent) : MyMainWindow("", "", parent) {}

MyMainWindow::MyMainWindow(const QString& inputFileName,
                           const QString& outputFileName,
                           QWidget* parent)
    : QMainWindow(parent),
      InputFileName{inputFileName},
      OutputFileName{outputFileName} {
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
    auto tabWidget = new QTabWidget;

    tabWidget->addTab(CreateViewTabWidget(), "View");
    tabWidget->addTab(CreatePointsTabWidget(), "Points");

    return tabWidget;
}

QWidget* MyMainWindow::CreateViewTabWidget() {
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

QWidget* MyMainWindow::CreatePointsTabWidget() {
    const auto sizePolicy = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto widget = new QWidget;
    auto mainLayout = new QVBoxLayout;

    auto openButton = new QPushButton;
    auto saveButton = new QPushButton;

    FirstCurvePoints = new MyPointsControlWidget("First curve:");
    SecondCurvePoints = new MyPointsControlWidget("Second curve:");

    auto initButton = [&sizePolicy](auto&& button, auto&& iconPath) {
        QPixmap pixmap(iconPath);
        QIcon icon(pixmap);

        button->setSizePolicy(sizePolicy);
        button->setIcon(icon);
        button->setIconSize(pixmap.rect().size());
    };

    initButton(openButton, ":/icons/openIcon.svg");
    initButton(saveButton, ":/icons/saveIcon.svg");

    connect(openButton, &QPushButton::clicked, this, [this]() {
        QString inputFileName;
        if (InputFileName != "") {
            inputFileName = InputFileName;
        } else {
            inputFileName = QFileDialog::getOpenFileName(
                this, "Load points", "", "JSON files (*.json);;All files (*)");
        }
        auto inputFile = QFile(inputFileName);
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Cannot open file!",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        try {
            PointsDocument pointsDocument(inputFile);
            FirstCurvePoints->SetPoints(
                pointsDocument.GetPoints(CurveType::FIRST));
            SecondCurvePoints->SetPoints(
                pointsDocument.GetPoints(CurveType::SECOND));
        } catch (std::runtime_error& e) {
            QMessageBox::warning(this, "Error", "Cannot parse file!",
                                 QMessageBox::Ok, QMessageBox::NoButton);
        }
    });

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QString outputFileName;
        if (OutputFileName != "") {
            outputFileName = OutputFileName;
        } else {
            outputFileName = QFileDialog::getSaveFileName(
                this, "Save points", "", "JSON files (*.json);;All files (*)");
        }
        auto outputFile = QFile(outputFileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Cannot open file!",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        PointsDocument pointsDocument;
        pointsDocument.SetPoints(CurveType::FIRST,
                                 FirstCurvePoints->GetPoints());
        pointsDocument.SetPoints(CurveType::SECOND,
                                 SecondCurvePoints->GetPoints());
        pointsDocument.Save(outputFile);
        outputFile.close();
    });

    auto fileLayout = new QHBoxLayout;
    fileLayout->addWidget(openButton);
    fileLayout->addWidget(saveButton);
    fileLayout->addStretch();

    mainLayout->addLayout(fileLayout);
    mainLayout->addWidget(FirstCurvePoints);
    mainLayout->addWidget(SecondCurvePoints);
    mainLayout->addStretch();

    widget->setLayout(mainLayout);

    return widget;
}
