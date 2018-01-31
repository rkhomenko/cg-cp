// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_POINTS_DOCUMNET_HPP_
#define CG_CP_POINTS_DOCUMENT_HPP_

#include <Vertex.hpp>

#include <QFile>

enum class CurveType { FIRST, SECOND };

using PointArray = std::array<Vertex, 4>;

class PointsDocument {
public:
    PointsDocument() = default;
    PointsDocument(QFile& inputFileName);

    void Load(QFile& inputFile);
    void Save(QFile& outputFile);

    void SetPoints(CurveType curveType, const PointArray& points);
    const PointArray& GetPoints(CurveType curveType) const;

private:
    static constexpr auto CURVES = "curves";
    static constexpr auto CURVE_ONE = "curve1";
    static constexpr auto CURVE_TWO = "curve2";
    static constexpr auto X = "x";
    static constexpr auto Y = "y";
    static constexpr auto Z = "z";

    PointArray FirstCurvePoints;
    PointArray SecondCurvePoints;
};

#endif  // CG_CP_POINTS_DOCUMENT_HPP_
