// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <PointsDocument.hpp>

#include <stdexcept>
#include <vector>

#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QVariant>

PointsDocument::PointsDocument(QFile& inputFileName) {
    Load(inputFileName);
}

void PointsDocument::Load(QFile& inputFile) {
    auto byteArray = inputFile.readAll();
    auto jsonError = QJsonParseError();
    auto jsonDocument = QJsonDocument::fromJson(byteArray, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qCritical() << "Cannot parse file" << inputFile.fileName() << ":"
                    << jsonError.errorString();
        throw std::runtime_error("cannot parse file");
    }

    auto extractPoints = [](auto&& pointsArray) {
        PointArray points;
        if (pointsArray.size() != 4) {
            qCritical() << "Bad config file!";
            throw std::runtime_error("bad config file");
        }

        auto i = 0UL;
        for (auto&& point : pointsArray) {
            auto pointObject = point.toObject();
            auto vertex =
                Vertex(pointObject[X].toDouble(), pointObject[Y].toDouble(),
                       pointObject[Z].toDouble());
            points[i++] = vertex;
        }

        return points;
    };

    auto object = jsonDocument.object();
    auto curvesObj = object.value(CURVES).toObject();
    auto pointsFirst = extractPoints(curvesObj.value(CURVE_ONE).toArray());
    auto pointsSecond = extractPoints(curvesObj.value(CURVE_TWO).toArray());

    SetPoints(CurveType::FIRST, pointsFirst);
    SetPoints(CurveType::SECOND, pointsSecond);
}

void PointsDocument::Save(QFile& outputFile) {
    QJsonObject mainObject;
    QJsonObject curves;

    for (auto type :
         std::vector<CurveType>{CurveType::FIRST, CurveType::SECOND}) {
        PointArray points;
        const char* curve;

        switch (type) {
            case CurveType::FIRST:
                curve = CURVE_ONE;
                std::copy(FirstCurvePoints.begin(), FirstCurvePoints.end(),
                          points.begin());
                break;
            case CurveType::SECOND:
                curve = CURVE_TWO;
                std::copy(SecondCurvePoints.begin(), SecondCurvePoints.end(),
                          points.begin());
                break;
        }

        QJsonArray curvePoints;
        for (auto&& point : points) {
            auto coords = point.GetPosition();

            QJsonObject pointObj;
            pointObj.insert(X, QJsonValue::fromVariant(coords[0]));
            pointObj.insert(Y, QJsonValue::fromVariant(coords[1]));
            pointObj.insert(Z, QJsonValue::fromVariant(coords[2]));

            curvePoints.push_back(pointObj);
        }

        curves.insert(curve, curvePoints);
    }

    mainObject.insert(CURVES, curves);

    QJsonDocument doc(mainObject);
    outputFile.write(doc.toJson());
}

void PointsDocument::SetPoints(CurveType curveType, const PointArray& points) {
    switch (curveType) {
        case CurveType::FIRST:
            std::copy(points.begin(), points.end(), FirstCurvePoints.begin());
            break;
        case CurveType::SECOND:
            std::copy(points.begin(), points.end(), SecondCurvePoints.begin());
            break;
    }
}

const PointArray& PointsDocument::GetPoints(CurveType curveType) const {
    if (curveType == CurveType::FIRST) {
        return FirstCurvePoints;
    }
    return SecondCurvePoints;
}
