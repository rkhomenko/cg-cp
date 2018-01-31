// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <MyMainWindow.hpp>
#include <PointsDocument.hpp>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>

void Init() {
    Q_INIT_RESOURCE(resources);

    QCoreApplication::setApplicationName("CG CP by Roman Khomenko (8O-308)");
    QCoreApplication::setOrganizationName("Khomenko and Co");
    QCoreApplication::setApplicationVersion("0.1.0");
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    Init();

    QCommandLineParser parser;
    parser.setApplicationDescription("CG CP by Roman Khomenko (8O-308)");
    QCommandLineOption inputFileOption(QStringList() << "i"
                                                     << "intput",
                                       "Set input file.", "file");
    QCommandLineOption outputFileOption(QStringList() << "o"
                                                      << "output",
                                        "Set output file.", "file");
    parser.addOption(inputFileOption);
    parser.addOption(outputFileOption);
    parser.addVersionOption();
    parser.addHelpOption();

    parser.process(a);

    PointsDocument pointsDocument;

    if (parser.isSet(inputFileOption)) {
        auto inputFileName = parser.value(inputFileOption);
        auto inputFile = QFile(inputFileName);
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Input file not set: cannot openfile" << inputFileName
                       << "!";
        }
        pointsDocument.Load(inputFile);
    }

    if (parser.isSet(outputFileOption)) {
        auto outputFileName = parser.value(outputFileOption);
        auto outputFile = QFile(outputFileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Output file not set: cannot openfile"
                       << outputFileName << "!";
        }
        pointsDocument.Save(outputFile);
    }

    MyMainWindow w;
    w.show();

    return a.exec();
}
