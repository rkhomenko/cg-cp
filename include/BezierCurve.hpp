// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_BEZIER_CURVE_HPP_
#define CG_CP_BEZIER_CURVE_HPP_

#include <Vertex.hpp>

#include <array>
#include <cstdint>
#include <vector>

#ifdef EIGEN3_INCLUDE_DIR
#include <Eigen/Dense>
#else
#include <eigen3/Eigen/Dense>
#endif

using Vec3 = Eigen::Matrix<float, 1, 3>;
using Vec4 = Eigen::Matrix<float, 1, 4>;
using Mat4x4 = Eigen::Matrix<float, 4, 4>;
using Map4x4 = Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>;

using SizeType = std::size_t;
using LenghtType = float;
using VertexVector = std::vector<Vertex>;

class BezierCurve {
public:
    template <class... Args>
    BezierCurve(Args... args) : Points{args...} {}

    Vec4 operator()(float t) const;

private:
    static const Mat4x4 BasisMatrix;

    static Vec4 GenPolynomVec(float t);

    std::array<Vec4, 4> Points;
};

#endif // CG_CP_BEZIER_CURVE_HPP_
