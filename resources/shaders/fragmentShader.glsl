// Computer graphic lab 6
// Variant 20
// Copyright © 2017-2018 Roman Khomenko (8O-308)
// All rights reserved

#version 330

varying highp vec4 normal;
varying highp vec4 point;

uniform highp float ambientCoeff;
uniform highp float diffuseCoeff;
uniform highp float specularCoeff;
uniform highp vec4 diffuseColor;
uniform highp vec3 light = vec3(0, 0, 1);
uniform highp vec3 toObserverVec = vec3(0, 0, 1);

const highp vec3 color = vec3(0.0f, 0.0f, 1.0f);
const highp float shineCoeff = 1.0f;

void main() {
    vec3 point3 = point.xyz;
    vec3 normal3 = normal.xyz;
    vec3 diffuseColor3 = diffuseColor.xyz;

    vec3 ambientI = ambientCoeff * color;
    vec3 fromPointToLightVec = light - point3;
    vec3 diffuseI = diffuseCoeff *
                    max(dot(fromPointToLightVec, normal3), 1.f) *
                    diffuseColor3;

    vec3 reflectedLightVec =
            2 * dot(normal3, fromPointToLightVec) * normal3 - fromPointToLightVec;
    vec3 specularI = specularCoeff *
                     pow(dot(reflectedLightVec, toObserverVec), shineCoeff) *
                     color;

    vec3 I = ambientI + diffuseI + specularI;
    gl_FragColor = vec4(I.xyz, 1);
}
