// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_ELLIPSOID_HPP_
#define CG_LAB_ELLIPSOID_HPP_

#include <Vertex.hpp>

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

class Layer {
public:
    enum class LayerType { SIDE, BOTTOM };

    Layer() = default;
    Layer(LenghtType a,
          LenghtType b,
          LenghtType c,
          LenghtType h,
          SizeType n,
          LenghtType deltaH,
          const Mat4x4& transformMatrix,
          const Vec3& viewPoint);
    Layer(LenghtType a,
          LenghtType b,
          LenghtType c,
          LenghtType h,
          SizeType n,
          const Mat4x4& transformMatrix,
          const Vec3& viewPoint);

    const VertexVector& GetVertices() const;
    SizeType GetItemsCount() const;
    Layer ApplyMatrix(const Mat4x4& matrix) const;
    LayerType GetType() const { return Type; }

private:
    static const float PI;

    void GenerateVertices(LenghtType a,
                          LenghtType b,
                          LenghtType c,
                          LenghtType h,
                          SizeType n,
                          LenghtType deltaH,
                          const Mat4x4& rotateMatrix,
                          const Vec3& viewPoint);
    void GenerateVertices(LenghtType a,
                          LenghtType b,
                          LenghtType c,
                          LenghtType h,
                          SizeType n,
                          const Mat4x4& rotateMatrix,
                          const Vec3& viewPoint);

    static Vec3 ToVec3(const Vec4& vec) { return Vec3(vec[0], vec[1], vec[2]); }
    static Vec4 ToVec4(const Vec3& vec) {
        return Vec4(vec[0], vec[1], vec[2], 1);
    }
    static Vec3 GetNormal(const Vec4& first,
                          const Vec4& middle,
                          const Vec4& last);
    static bool CheckNormal(const Vec3& normal, const Vec3& viewPoint);

    VertexVector Vertices;
    LayerType Type;
};

using LayerVector = std::vector<Layer>;

class Ellipsoid {
public:
    Ellipsoid() = default;
    Ellipsoid(LenghtType a,
              LenghtType b,
              LenghtType c,
              SizeType vertexCount,
              SizeType surfaceCount,
              const Vec3& viewPoint);

    SizeType GetVertexCount() const;
    LayerVector GenerateVertices(const Mat4x4& rotateMatrix) const;

    void SetVertexCount(SizeType count);
    void SetSurfaceCount(SizeType count);

private:
    static LayerVector ApplyMatrix(const LayerVector& layers,
                                   const Mat4x4& matrix);

    LenghtType A;
    LenghtType B;
    LenghtType C;
    SizeType VertexCount;
    SizeType SurfaceCount;
    Vec3 ViewPoint;
};

#endif  // CG_LAB_ELLIPSOID_HPP_
