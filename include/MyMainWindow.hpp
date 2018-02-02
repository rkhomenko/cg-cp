// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_MYMAINWINDOW_HPP_
#define CG_CP_MYMAINWINDOW_HPP_

#include <MyPointsControlWidget.hpp>

#include <array>

#include <QMainWindow>
#include <QString>

class MyOpenGLWidget;

class MyMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget* parent = nullptr);
    explicit MyMainWindow(const QString& inputFileName,
                          const QString& outputFileName,
                          QWidget* parent = nullptr);
    ~MyMainWindow() = default;

    static constexpr auto VARIANT_DESCRIPTION =
        "Computer grapics curse project\n"
        "Variant 20: ruled surface\n"
        "\t(guides - Bezier curves)\n"
        "Made by Roman Khomenko (8O-308)";

private:
    QWidget* CreateCentralWidget();
    QWidget* CreateViewTabWidget();
    QWidget* CreatePointsTabWidget();

    MyOpenGLWidget* OpenGLWidget;
    QString InputFileName;
    QString OutputFileName;
    MyPointsControlWidget* FirstCurvePoints;
    MyPointsControlWidget* SecondCurvePoints;
};

#endif  // CG_CP_MYMAINWINDOW_HPP_
