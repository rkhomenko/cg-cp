// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyMainWindow.hpp>

#include <QApplication>

void Init() {
    Q_INIT_RESOURCE(resources);

    QCoreApplication::setApplicationName("CG Lab06 by Roman Khomenko (8O-308)");
    QCoreApplication::setOrganizationName("Khomenko and Co");
    QCoreApplication::setApplicationVersion("0.1.0");
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    Init();

    MyMainWindow w;
    w.show();

    return a.exec();
}
