// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_LAB_MYOPENGLWIDGET_HPP_
#define CG_LAB_MYOPENGLWIDGET_HPP_

#include <Ellipsoid.hpp>

#include <array>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLShaderProgram;

class QTimer;

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    using FloatType = float;

    explicit MyOpenGLWidget(QWidget* parent = nullptr);
    explicit MyOpenGLWidget(LenghtType a,
                            LenghtType b,
                            LenghtType c,
                            SizeType vertexCount,
                            SizeType surfaceCount,
                            QWidget* parent = nullptr);
    ~MyOpenGLWidget();

public slots:
    void ScaleUpSlot();
    void ScaleDownSlot();

    void OXAngleChangedSlot(FloatType angle);
    void OYAngleChangedSlot(FloatType angle);
    void OZAngleChangedSlot(FloatType angle);

    void AmbientChangedSlot(float ambientCoeff);
    void SpecularChangedSlot(float specularCoeff);
    void DiffuseChangedSlot(float diffuseCoeff);

    void VertexCountChangedSlot(int count);
    void SurfaceCountChangedSlot(int count);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private slots:
    void CleanUp();
    void OnTimeoutSlot();

private:
    enum RotateType { OX, OY, OZ };

    static constexpr auto WIDGET_DEFAULT_SIZE = QSize(350, 350);
    static constexpr auto IMAGE_DEFAULT_SIZE = QSize(300, 300);
    static const Vec3 VIEW_POINT;
    static const float PI;

    static constexpr auto VERTEX_SHADER = ":/shaders/vertexShader.glsl";
    static constexpr auto FRAGMENT_SHADER = ":/shaders/fragmentShader.glsl";
    static constexpr auto POSITION = "position";
    static constexpr auto COLOR = "color";
    static constexpr auto TRANSFORM_MATRIX = "transformMatrix";
    static constexpr auto AMBIENT_COEFF = "ambientCoeff";
    static constexpr auto DIFFUSE_COEFF = "diffuseCoeff";
    static constexpr auto SPECULAR_COEFF = "specularCoeff";
    static constexpr auto DIFFUSE_COLOR = "diffuseColor";

    static constexpr auto SCALE_FACTOR_PER_ONCE = 1.15f;

    static SizeType GetVertexCount(const LayerVector& layers);

    void UpdateOnChange(int width, int height);
    void OnWidgetUpdate();

    Mat4x4 GenerateScaleMatrix(int width, int height) const;
    Mat4x4 GenerateRotateMatrix(RotateType rotateType) const;

    void SetUniformMatrix(const Mat4x4& transformMatrix);
    void SetUniformValue(const char* name, float value);

    static Mat4x4 GenerateRotateMatrixByAngle(RotateType rotateType,
                                              FloatType angle);
    static Mat4x4 GenerateProjectionMatrix();

    QOpenGLShaderProgram* ShaderProgram;
    QOpenGLBuffer* Buffer;
    QOpenGLVertexArrayObject* VertexArray;
    Ellipsoid EllipsoidLayer;
    FloatType ScaleFactor;
    FloatType AngleOX;
    FloatType AngleOY;
    FloatType AngleOZ;
    FloatType AmbientCoeff;
    FloatType SpecularCoeff;
    FloatType DiffuseCoeff;
    FloatType A;
    FloatType B;
    FloatType C;
    SizeType VertexCount;
    SizeType SurfaceCount;
    LayerVector Layers;
    QTimer* Timer;
    FloatType Teta;
    FloatType Phi;
    FloatType Red;
    FloatType Green;
    FloatType Blue;
};

#endif  // CG_LAB_MYOPENGLWIDGET_HPP_
