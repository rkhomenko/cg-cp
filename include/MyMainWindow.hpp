// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_MYMAINWINDOW_HPP_
#define CG_CP_MYMAINWINDOW_HPP_

#include <QMainWindow>

#include <array>

class MyOpenGLWidget;

class MyMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget* parent = nullptr);
    ~MyMainWindow() = default;

    static constexpr auto VARIANT_DESCRIPTION =
        "Computer grapics curse project\n"
        "Variant 20: ellipsoid layer with сolor\n"
        "changing by sinusoidal law\n"
        "Made by Roman Khomenko (8O-308)";

private:
    QWidget* CreateCentralWidget();

    MyOpenGLWidget* OpenGLWidget;
};

#endif  // CG_CP_MYMAINWINDOW_HPP_