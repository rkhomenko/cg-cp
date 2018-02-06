// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <BezierCurve.hpp>
#include <MyMainWindow.hpp>
#include <MyOpenGLWidget.hpp>

#include <cmath>

#include <QApplication>
#include <QDebug>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QResizeEvent>

const float MyOpenGLWidget::PI = 4.0f * std::atan(1.0f);

using Pair = MyOpenGLWidget::Pair;

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      ScaleFactor{1.0f},
      AngleOX{0.0},
      AngleOY{0.0},
      AngleOZ{0.0},
      PointsCount{20},
      LinesCount{20},
      Teta{0},
      Phi{0} {
    auto sizePolicy =
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setMinimumSize(WIDGET_DEFAULT_SIZE);
}

MyOpenGLWidget::~MyOpenGLWidget() {}

void MyOpenGLWidget::ScaleUpSlot() {
    ScaleFactor *= SCALE_FACTOR_PER_ONCE;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::ScaleDownSlot() {
    ScaleFactor /= SCALE_FACTOR_PER_ONCE;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OXAngleChangedSlot(FloatType angle) {
    AngleOX = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OYAngleChangedSlot(FloatType angle) {
    AngleOY = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::OZAngleChangedSlot(FloatType angle) {
    AngleOZ = angle;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::PointsCountChangedSlot(int pointsCount) {
    PointsCount = static_cast<SizeType>(pointsCount);
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::LinesCountChangedSlot(int linesCount) {
    LinesCount = static_cast<SizeType>(linesCount);
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::PointsChanged(const PointArray& curve1,
                                   const PointArray& curve2) {
    FirstCurve = curve1;
    SecondCurve = curve2;
}

void MyOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
            &MyOpenGLWidget::CleanUp);

    ShaderProgram = new QOpenGLShaderProgram(this);
    ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           VERTEX_SHADER);
    ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           FRAGMENT_SHADER);

    if (!ShaderProgram->link()) {
        qDebug() << ShaderProgram->log();
        QApplication::quit();
    }

    Buffer = new QOpenGLBuffer;
    VertexArray = new QOpenGLVertexArrayObject;

    UpdateOnChange(width(), height());

    VertexArray->release();
    Buffer->release();
    ShaderProgram->release();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    UpdateOnChange(width, height);
}

Vec4 MyOpenGLWidget::SurfacePoint(float u, float w) const noexcept {
    BezierCurve firstCurve(FirstCurve);
    BezierCurve secondCurve(SecondCurve);
    return (1 - w) * firstCurve(u) + w * secondCurve(u);
}

Pair MyOpenGLWidget::GenBuff1Points(int linesCount, int pointsCount) const
    noexcept {
    std::vector<Vertex> surfaceVertices;
    auto lineVerticesCount = 0ULL;
    float deltaU = 1.0f / pointsCount;
    float deltaW = 1.0f / linesCount;

    auto drawPoints = [&, this](float w) {
        if (pointsCount == 2) {
            surfaceVertices.push_back(SurfacePoint(0.0f, w));
            surfaceVertices.push_back(SurfacePoint(1.0f, w));
            return surfaceVertices.size();
        }

        for (auto u = 0.0f; u < 1.0f; u += deltaU) {
            surfaceVertices.push_back(SurfacePoint(u, w));
        }
        surfaceVertices.push_back(SurfacePoint(1.0f, w));
        return surfaceVertices.size();
    };

    if (linesCount == 2) {
        lineVerticesCount = drawPoints(0.0f);
        drawPoints(1.0f);
        return std::make_pair(surfaceVertices, lineVerticesCount);
    }

    for (auto w = 0.0f; w < 1.0f; w += deltaW) {
        drawPoints(w);
    }

    lineVerticesCount = surfaceVertices.size();
    drawPoints(1.0f);
    lineVerticesCount = surfaceVertices.size() - lineVerticesCount;

    return std::make_pair(surfaceVertices, lineVerticesCount);
}

Pair MyOpenGLWidget::GenBuff2Points(int linesCount, int pointsCount) const
    noexcept {
    std::vector<Vertex> surfaceVertices;
    auto lineVerticesCount = 0ULL;
    float deltaU = 1.0f / pointsCount;
    float deltaW = 1.0f / linesCount;

    auto drawPoints = [&, this](float u) {
        if (linesCount == 2) {
            surfaceVertices.push_back(SurfacePoint(u, 0.0f));
            surfaceVertices.push_back(SurfacePoint(u, 1.0f));
            return surfaceVertices.size();
        }

        for (auto w = 0.0f; w < 1.0f; w += deltaW) {
            surfaceVertices.push_back(SurfacePoint(u, w));
        }
        surfaceVertices.push_back(SurfacePoint(u, 1.0f));
        return surfaceVertices.size();
    };

    if (linesCount == 2) {
        lineVerticesCount = drawPoints(0.0f);
        drawPoints(1.0f);
        return std::make_pair(surfaceVertices, lineVerticesCount);
    }

    for (auto u = 0.0f; u < 1.0f; u += deltaU) {
        drawPoints(u);
    }

    lineVerticesCount = surfaceVertices.size();
    drawPoints(1.0f);
    lineVerticesCount = surfaceVertices.size() - lineVerticesCount;

    return std::make_pair(surfaceVertices, lineVerticesCount);
}

void MyOpenGLWidget::paintGL() {
    if (!ShaderProgram->bind()) {
        qDebug() << "Cannot bind program";
        QApplication::quit();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto v1 = GenBuff1Points(LinesCount, PointsCount);
    auto v2 = GenBuff2Points(LinesCount, PointsCount);

    std::vector<Vertex> surfaceVertices;
    surfaceVertices.reserve(v1.first.size() + v2.first.size());
    surfaceVertices.insert(surfaceVertices.begin(), v1.first.begin(),
                           v1.first.end());
    surfaceVertices.insert(surfaceVertices.end(), v2.first.begin(),
                           v2.first.end());

    Buffer->destroy();
    if (!Buffer->create()) {
        qDebug() << "Cannot create buffer!";
    }
    if (!Buffer->bind()) {
        qDebug() << "Cannot bind buffer!";
    }
    Buffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    Buffer->allocate(surfaceVertices.data(),
                     sizeof(Vertex) * surfaceVertices.size());

    VertexArray->destroy();
    VertexArray->create();
    VertexArray->bind();

    int posAttr = ShaderProgram->attributeLocation(POSITION);
    int colorAttr = ShaderProgram->attributeLocation(COLOR);
    ShaderProgram->enableAttributeArray(posAttr);
    ShaderProgram->setAttributeBuffer(
        posAttr, GL_FLOAT, Vertex::GetPositionOffset(),
        Vertex::GetPositionTupleSize(), Vertex::GetStride());
    ShaderProgram->enableAttributeArray(colorAttr);
    ShaderProgram->setAttributeBuffer(
        colorAttr, GL_FLOAT, Vertex::GetColorOffset(),
        Vertex::GetColorTupleSize(), Vertex::GetStride());

    auto count1 = v1.first.size() / v1.second +
                  ((v1.first.size() % v1.second != 0) ? 1 : 0);
    auto count2 =
        v2.first.size() / v2.second +
        ((v2.first.size() % v2.second != 0) ? 1 : 0);
    auto offset = 0;
    for (auto i = 0UL; i < count1; i++) {
        glDrawArrays(GL_LINE_STRIP, offset, v1.second);
        offset += v1.second;
    }
    for (auto i = 0UL; i < count2; i++) {
        glDrawArrays(GL_LINE_STRIP, offset, v2.second);
        offset += v2.second;
    }

    Buffer->release();
    VertexArray->release();
    ShaderProgram->release();
}

void MyOpenGLWidget::CleanUp() {
    VertexArray->destroy();
    Buffer->destroy();
    delete VertexArray;
    delete Buffer;
    delete ShaderProgram;
}

void MyOpenGLWidget::UpdateOnChange(int width, int height) {
    const Mat4x4 rotateMatrix = GenerateRotateMatrix(RotateType::OX) *
                                GenerateRotateMatrix(RotateType::OY) *
                                GenerateRotateMatrix(RotateType::OZ);
    const Mat4x4 projectionMatrix = GenerateProjectionMatrix();
    const Mat4x4 scaleMatrix = GenerateScaleMatrix(width, height);
    const Mat4x4 transformMatrix =
        scaleMatrix * projectionMatrix * rotateMatrix;

    SetUniformMatrix(transformMatrix);
}

void MyOpenGLWidget::OnWidgetUpdate() {
    auto event = new QResizeEvent(size(), size());
    QOpenGLWidget::event(event);
}

Mat4x4 MyOpenGLWidget::GenerateScaleMatrix(int width, int height) const {
    auto xScaleFactor = 1.0f / (0.5f * width);
    auto yScaleFactor = 1.0f / (0.5f * height);

    GLfloat matrixData[] = {
        xScaleFactor * ScaleFactor,
        0.0f,
        0.0f,
        0.0f,  // first line
        0.0f,
        yScaleFactor * ScaleFactor,
        0.0f,
        0.0f,  // second line
        0.0f,
        0.0f,
        1.0f,
        0.0f,  // third line
        0.0f,
        0.0f,
        0.0f,
        1.0f  // fourth line
    };

    return Map4x4(matrixData);
}

Mat4x4 MyOpenGLWidget::GenerateRotateMatrix(RotateType rotateType) const {
    FloatType angle = 0;
    switch (rotateType) {
        case RotateType::OX:
            angle = AngleOX;
            break;
        case RotateType::OY:
            angle = AngleOY;
            break;
        case RotateType::OZ:
            angle = AngleOZ;
            break;
    }
    return GenerateRotateMatrixByAngle(rotateType, angle);
}

Mat4x4 MyOpenGLWidget::GenerateRotateMatrixByAngle(RotateType rotateType,
                                                   FloatType angle) {
    FloatType rotateOXData[] = {
        1.0f,
        0,
        0,
        0,  // first line
        0,
        std::cos(angle),
        std::sin(angle),
        0,  // second line
        0,
        -std::sin(angle),
        std::cos(angle),
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType rotateOYData[] = {
        std::cos(angle),
        0,
        -std::sin(angle),
        0,  // fist line
        0,
        1.0f,
        0,
        0,  // second line
        std::sin(angle),
        0,
        std::cos(angle),
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType rotateOZData[] = {
        std::cos(angle),
        std::sin(angle),
        0,
        0,  // first line
        -std::sin(angle),
        std::cos(angle),
        0,
        0,  // second line
        0,
        0,
        1.0f,
        0,  // third line
        0,
        0,
        0,
        1.0f  // fourth line
    };

    FloatType* matrixData = nullptr;
    switch (rotateType) {
        case RotateType::OX:
            matrixData = rotateOXData;
            break;
        case RotateType::OY:
            matrixData = rotateOYData;
            break;
        case RotateType::OZ:
            matrixData = rotateOZData;
            break;
    }

    return Map4x4(matrixData);
}

Mat4x4 MyOpenGLWidget::GenerateProjectionMatrix() {
    FloatType matrixData[] = {
        1, 0, 0, 0,  // first line
        0, 1, 0, 0,  // second line
        0, 0, 0, 0,  // third line
        0, 0, 0, 1   // fourth line
    };

    return Map4x4(matrixData);
}

void MyOpenGLWidget::SetUniformMatrix(const Mat4x4& transformMatrix) {
    ShaderProgram->bind();

    ShaderProgram->setUniformValue(TRANSFORM_MATRIX,
                                   QMatrix4x4(transformMatrix.data()));
    ShaderProgram->release();
}

void MyOpenGLWidget::SetUniformValue(const char* name, float value) {
    ShaderProgram->bind();
    ShaderProgram->setUniformValue(name, value);
    ShaderProgram->release();
}
