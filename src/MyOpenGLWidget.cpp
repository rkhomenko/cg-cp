// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#include <Ellipsoid.hpp>
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
#include <QTimer>

const Vec3 MyOpenGLWidget::VIEW_POINT = Vec3(0, 0, 1);
const float MyOpenGLWidget::PI = 4.0f * std::atan(1.0f);

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
    : MyOpenGLWidget(0.5, 0.5, 0.5, 4, 5, parent) {}

MyOpenGLWidget::MyOpenGLWidget(LenghtType a,
                               LenghtType b,
                               LenghtType c,
                               SizeType vertexCount,
                               SizeType surfaceCount,
                               QWidget* parent)
    : QOpenGLWidget(parent),
      EllipsoidLayer{a, b, c, vertexCount, surfaceCount, VIEW_POINT},
      ScaleFactor{3.0f},
      AngleOX{0.0},
      AngleOY{0.0},
      AngleOZ{0.0},
      AmbientCoeff{0.2},
      SpecularCoeff{0.2},
      DiffuseCoeff{0.3},
      A{a},
      B{b},
      C{c},
      VertexCount{vertexCount},
      SurfaceCount{surfaceCount},
      Teta{0},
      Phi{0} {
    auto sizePolicy =
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setMinimumSize(WIDGET_DEFAULT_SIZE);

    Timer = new QTimer;
    connect(Timer, &QTimer::timeout, this, &MyOpenGLWidget::OnTimeoutSlot);
}

MyOpenGLWidget::~MyOpenGLWidget() {
    delete Timer;
}

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

void MyOpenGLWidget::AmbientChangedSlot(float ambientCoeff) {
    AmbientCoeff = ambientCoeff;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::SpecularChangedSlot(float specularCoeff) {
    SpecularCoeff = specularCoeff;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::DiffuseChangedSlot(float diffuseCoeff) {
    DiffuseCoeff = diffuseCoeff;
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::VertexCountChangedSlot(int count) {
    VertexCount = static_cast<SizeType>(count);
    UpdateOnChange(width(), height());
    OnWidgetUpdate();
}

void MyOpenGLWidget::SurfaceCountChangedSlot(int count) {
    SurfaceCount = static_cast<SizeType>(count);
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
    UpdateOnChange(width(), height());
    Buffer->allocate(GetVertexCount(Layers) * sizeof(Vertex));

    {
        int offset = 0;
        for (auto&& layer : Layers) {
            auto& vertices = layer.GetVertices();
            auto bytes = vertices.size() * sizeof(Vertex);
            Buffer->write(offset, vertices.data(), bytes);
            offset += bytes;
        }
    }

    VertexArray = new QOpenGLVertexArrayObject;
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

    ShaderProgram->disableAttributeArray(posAttr);
    ShaderProgram->disableAttributeArray(colorAttr);

    VertexArray->release();
    Buffer->release();

    Timer->start(1000);
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
    glShadeModel(GL_SMOOTH);

    Buffer->destroy();
    if (!Buffer->create()) {
        qDebug() << "Cannot create buffer";
    }

    if (!Buffer->bind()) {
        qDebug() << "Cannot bind buffer";
    }
    Buffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    Buffer->allocate(GetVertexCount(Layers) * sizeof(Vertex));
    {
        int offset = 0;
        for (auto&& layer : Layers) {
            auto& vertices = layer.GetVertices();
            auto bytes = vertices.size() * sizeof(Vertex);
            Buffer->write(offset, vertices.data(), bytes);
            offset += bytes;
        }
    }

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
    {
        int offset = 0;
        for (auto&& layer : Layers) {
            int count = layer.GetItemsCount();
            glDrawArrays(GL_TRIANGLES, offset, count);
            offset += count;
        }
    }

    ShaderProgram->disableAttributeArray(posAttr);
    ShaderProgram->disableAttributeArray(colorAttr);

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

void MyOpenGLWidget::OnTimeoutSlot() {
    const auto PI = 4.0f * std::atan(1.0f);
    const auto delta = PI / 100;

    if (Red < PI / 2) {
        Red += delta;
    } else if (Green < PI / 2) {
        Green += delta;
    } else if (Blue < PI / 2) {
        Blue += delta;
    } else {
        Red = Green = Blue = 0;
    }

    auto col = QVector4D(std::sin(Red), std::sin(Green), std::sin(Blue), 1);

    ShaderProgram->bind();
    ShaderProgram->setUniformValue(DIFFUSE_COLOR, col);
    ShaderProgram->release();

    UpdateOnChange(width(), height());
    OnWidgetUpdate();
    repaint();

    Timer->start(100);
}

SizeType MyOpenGLWidget::GetVertexCount(const LayerVector& layers) {
    SizeType result = 0;
    for (auto&& layer : layers) {
        result += layer.GetVertices().size();
    }
    return result;
}

void MyOpenGLWidget::UpdateOnChange(int width, int height) {
    const Mat4x4 rotateMatrix = GenerateRotateMatrix(RotateType::OX) *
                                GenerateRotateMatrix(RotateType::OY) *
                                GenerateRotateMatrix(RotateType::OZ);
    const Mat4x4 projectionMatrix = GenerateProjectionMatrix();
    const Mat4x4 scaleMatrix = GenerateScaleMatrix(width, height);
    const Mat4x4 transformMatrix = scaleMatrix * projectionMatrix;

    EllipsoidLayer.SetVertexCount(VertexCount);
    EllipsoidLayer.SetSurfaceCount(SurfaceCount);
    Layers = EllipsoidLayer.GenerateVertices(rotateMatrix);
    SetUniformMatrix(transformMatrix);
    SetUniformValue(AMBIENT_COEFF, AmbientCoeff);
    SetUniformValue(DIFFUSE_COEFF, DiffuseCoeff);
    SetUniformValue(SPECULAR_COEFF, SpecularCoeff);
}

void MyOpenGLWidget::OnWidgetUpdate() {
    auto event = new QResizeEvent(size(), size());
    QOpenGLWidget::event(event);
}

Mat4x4 MyOpenGLWidget::GenerateScaleMatrix(int width, int height) const {
    const auto DEFAULT_WIDTH = IMAGE_DEFAULT_SIZE.width();
    const auto DEFAULT_HEIGHT = IMAGE_DEFAULT_SIZE.height();

    auto xScaleFactor = 1.0f * DEFAULT_WIDTH / width;
    auto yScaleFactor = 1.0f * DEFAULT_HEIGHT / height;

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
