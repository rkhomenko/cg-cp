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

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      ScaleFactor{1.0f},
      AngleOX{0.0},
      AngleOY{0.0},
      AngleOZ{0.0},
      LinesCount{0},
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

void MyOpenGLWidget::LineCountChangedSlot(int count) {
    LinesCount = static_cast<SizeType>(count);
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
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
    Buffer->create();
    Buffer->bind();
    Buffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);

    Vertex triangle[] = {Vertex{-175.f, -175.0f},
                         Vertex{0.0f, 350.0f * std::sqrt(3) / 2 - 175.0f},
                         Vertex{175.0f, -175.0f}};

    Buffer->allocate(triangle, 3 * sizeof(Vertex));

    VertexArray = new QOpenGLVertexArrayObject;
    VertexArray->create();
    VertexArray->bind();

    int posAttr = ShaderProgram->attributeLocation(POSITION);
    ShaderProgram->enableAttributeArray(posAttr);
    ShaderProgram->setAttributeBuffer(
        posAttr, GL_FLOAT, Vertex::GetPositionOffset(),
        Vertex::GetPositionTupleSize(), Vertex::GetStride());

    ShaderProgram->disableAttributeArray(posAttr);

    UpdateOnChange(width(), height());

    VertexArray->release();
    Buffer->release();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    UpdateOnChange(width, height);
}

void MyOpenGLWidget::paintGL() {
    if (!ShaderProgram->bind()) {
        qDebug() << "Cannot bind program";
        QApplication::quit();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (!Buffer->bind()) {
        qDebug() << "Cannot bind buffer";
    }

    VertexArray->bind();
    int posAttr = ShaderProgram->attributeLocation(POSITION);
    ShaderProgram->enableAttributeArray(posAttr);

    glDrawArrays(GL_TRIANGLES, 0, 3);

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
