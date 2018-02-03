// Computer graphic curse project
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#version 330 core

attribute highp vec4 position;

uniform highp mat4x4 transformMatrix;

void main() {
    gl_Position = position * transformMatrix;
}
