#include <BezierCurve.hpp>

static float data[] = {
    -1.0f,  3.0f, -3.0f, 1.0f,
     3.0f, -6.0f,  3.0f, 0.0f,
    -3.0f,  3.0f,  0.0f, 0.0f,
     1.0f,  0.0f,  0.0f, 0.0f
};

const Mat4x4 BezierCurve::BasisMatrix = Map4x4(data);

Vec4 BezierCurve::operator()(float t) const {
    const Vec4 coeffs = GenPolynomVec(t) * BasisMatrix;
    auto result = Vec4(0, 0, 0, 0);
    for (auto i = 0UL; i < Points.size(); i++) {
        result += coeffs[i] * Points[i];
    }
    return result;
}

Vec4 BezierCurve::GenPolynomVec(float t) {
    return Vec4(t * t * t, t * t, t, 1);
}
