// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#version 330

attribute highp vec4 position;
attribute highp vec4 color;

uniform highp mat4x4 transformMatrix;

varying highp vec4 normal;
varying highp vec4 point;

void main() {
    point = position;
    normal = color;
    gl_Position = position * transformMatrix;
}
