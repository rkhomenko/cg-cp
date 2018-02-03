// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#ifndef CG_CP_MYOPENGLWIDGET_HPP_
#define CG_CP_MYOPENGLWIDGET_HPP_

#include <BezierCurve.hpp>

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
    ~MyOpenGLWidget();

public slots:
    void ScaleUpSlot();
    void ScaleDownSlot();

    void OXAngleChangedSlot(FloatType angle);
    void OYAngleChangedSlot(FloatType angle);
    void OZAngleChangedSlot(FloatType angle);

    void VertexCountChangedSlot(int count);
    void SurfaceCountChangedSlot(int count);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private slots:
    void CleanUp();

private:
    enum RotateType { OX, OY, OZ };

    static constexpr auto WIDGET_DEFAULT_SIZE = QSize(350, 350);
    static constexpr auto IMAGE_DEFAULT_SIZE = QSize(300, 300);
    static const float PI;

    static constexpr auto VERTEX_SHADER = ":/shaders/vertexShader.glsl";
    static constexpr auto FRAGMENT_SHADER = ":/shaders/fragmentShader.glsl";
    static constexpr auto POSITION = "position";
    static constexpr auto COLOR = "color";
    static constexpr auto TRANSFORM_MATRIX = "transformMatrix";

    static constexpr auto SCALE_FACTOR_PER_ONCE = 1.15f;

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
    FloatType ScaleFactor;
    FloatType AngleOX;
    FloatType AngleOY;
    FloatType AngleOZ;
    SizeType VertexCount;
    SizeType SurfaceCount;
    FloatType Teta;
    FloatType Phi;
};

#endif  // CG_CP_MYOPENGLWIDGET_HPP_
