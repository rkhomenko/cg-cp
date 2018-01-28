#include <Ellipsoid.hpp>

#include <algorithm>
#include <cmath>
#include <future>
#include <vector>

const float Layer::PI = 4 * std::atan(1.0f);

Layer::Layer(LenghtType a,
             LenghtType b,
             LenghtType c,
             LenghtType h,
             SizeType n,
             LenghtType deltaH,
             const Mat4x4& transformMatrix,
             const Vec3& viewPoint)
    : Type{LayerType::SIDE} {
    GenerateVertices(a, b, c, h, n, deltaH, transformMatrix, viewPoint);
}

Layer::Layer(LenghtType a,
             LenghtType b,
             LenghtType c,
             LenghtType h,
             SizeType n,
             const Mat4x4& transformMatrix,
             const Vec3& viewPoint)
    : Type{LayerType::BOTTOM} {
    GenerateVertices(a, b, c, h, n, transformMatrix, viewPoint);
}

const VertexVector& Layer::GetVertices() const {
    return Vertices;
}

SizeType Layer::GetItemsCount() const {
    return Vertices.size();
}

Layer Layer::ApplyMatrix(const Mat4x4& matrix) const {
    Layer layer;
    for (auto&& vertex : Vertices) {
        layer.Vertices.emplace_back(vertex.GetPosition() * matrix,
                                    vertex.GetColor());
    }
    return layer;
}

void Layer::GenerateVertices(LenghtType a,
                             LenghtType b,
                             LenghtType c,
                             LenghtType h,
                             SizeType n,
                             LenghtType deltaH,
                             const Mat4x4& rotateMatrix,
                             const Vec3& viewPoint) {
    const auto DELTA_PHI = 2 * PI / n;

    auto generateVertex = [a, b, c, DELTA_PHI](auto&& i, auto&& h) {
        const auto C = (c * c - h * h) / c * c;
        const auto A = std::sqrt(C) * a;
        const auto B = std::sqrt(C) * b;
        return Vertex(A * std::cos(i * DELTA_PHI), B * std::sin(i * DELTA_PHI),
                      h);
    };

    for (auto i = 0UL; i < n; i++) {
        auto first = generateVertex(i, h).GetPosition() * rotateMatrix;
        auto second =
            generateVertex(i, h + deltaH).GetPosition() * rotateMatrix;
        auto third = generateVertex(i + 1, h).GetPosition() * rotateMatrix;
        auto fourth =
            generateVertex(i + 1, h + deltaH).GetPosition() * rotateMatrix;

        Vec3 normal = GetNormal(first, second, third);
        if (CheckNormal(normal, viewPoint)) {
            Vertices.emplace_back(first, ToVec4(normal));
            Vertices.emplace_back(second, ToVec4(normal));
            Vertices.emplace_back(third, ToVec4(normal));
        }

        normal = GetNormal(second, fourth, third);
        if (CheckNormal(normal, viewPoint)) {
            Vertices.emplace_back(second, ToVec4(normal));
            Vertices.emplace_back(fourth, ToVec4(normal));
            Vertices.emplace_back(third, ToVec4(normal));
        }
    }
}

void Layer::GenerateVertices(LenghtType a,
                             LenghtType b,
                             LenghtType c,
                             LenghtType h,
                             SizeType n,
                             const Mat4x4& rotateMatrix,
                             const Vec3& viewPoint) {
    const auto DELTA_PHI = 2 * PI / n;

    auto generateVertex = [a, b, c, DELTA_PHI](auto&& i, auto&& h) {
        const auto C = (c * c - h * h) / c * c;
        const auto A = std::sqrt(C) * a;
        const auto B = std::sqrt(C) * b;
        return Vertex(A * std::cos(i * DELTA_PHI), B * std::sin(i * DELTA_PHI),
                      h);
    };

    const Vec4 center = Vec4(0, 0, h, 1) * rotateMatrix;

    for (auto i = 0UL; i < n; i++) {
        auto first = generateVertex(i, h).GetPosition() * rotateMatrix;
        auto second = generateVertex(i + 1, h).GetPosition() * rotateMatrix;

        Vec3 normal = GetNormal(first, center, second);
        if (CheckNormal(normal, viewPoint)) {
            Vertices.emplace_back(first, ToVec4(normal));
            Vertices.emplace_back(center, ToVec4(normal));
            Vertices.emplace_back(second, ToVec4(normal));
        }
    }
}

Vec3 Layer::GetNormal(const Vec4& first, const Vec4& middle, const Vec4& last) {
    const auto center = Vec3(0, 0, 0);
    auto v1 = ToVec3(middle - first);
    auto v2 = ToVec3(last - first);

    Vec3 normal = v1.cross(v2);
    normal.normalize();

    if (Vec3 toCenterVec = center - ToVec3(middle);
        toCenterVec.dot(normal) > 0) {
        normal *= -1.0f;
    }

    return normal;
}

bool Layer::CheckNormal(const Vec3& normal, const Vec3& viewPoint) {
    float dotProduct = viewPoint.dot(normal);
    if (dotProduct > 0) {
        return true;
    }
    return false;
}

Ellipsoid::Ellipsoid(LenghtType a,
                     LenghtType b,
                     LenghtType c,
                     SizeType vertexCount,
                     SizeType surfaceCount,
                     const Vec3& viewPoint)
    : A{a},
      B{b},
      C{c},
      VertexCount{vertexCount},
      SurfaceCount{surfaceCount},
      ViewPoint{viewPoint} {}

LayerVector Ellipsoid::GenerateVertices(const Mat4x4& rotateMatrix) const {
    LayerVector layers;
    float start = -0.1f;
    float stop = 0.1f;
    float delta = (stop - start) / SurfaceCount;
    auto height = start;

    std::vector<std::future<Layer>> futures;

    for (height = start; height <= stop; height += delta) {
        futures.emplace_back(std::async(
            std::launch::async,
            [](float a, float b, float c, float height, SizeType vertexCount,
               float delta, const Mat4x4& transformMatrix,
               const Vec3& viewPoint) {
                return Layer(a, b, c, height, vertexCount, delta,
                             transformMatrix, viewPoint);
            },
            A, B, C, height, VertexCount, delta, rotateMatrix, ViewPoint));
    }

    for (auto&& future : futures) {
        future.wait();
        auto layer = future.get();
        if (layer.GetItemsCount() != 0) {
            layers.emplace_back(layer);
        }
    }

    for (auto h : {start, height}) {
        auto layer =
            Layer(A, B, C, h, VertexCount, rotateMatrix, ViewPoint);
        if (layer.GetItemsCount() != 0) {
            layers.emplace_back(layer);
        }
    }
    return layers;
}

void Ellipsoid::SetVertexCount(SizeType count) {
    VertexCount = count;
}

void Ellipsoid::SetSurfaceCount(SizeType count) {
    SurfaceCount = count;
}

LayerVector Ellipsoid::ApplyMatrix(const LayerVector& layers,
                                   const Mat4x4& matrix) {
    LayerVector result;
    for (auto&& layer : layers) {
        result.emplace_back(layer.ApplyMatrix(matrix));
    }
    return result;
}
