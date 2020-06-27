/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: ReflectionMap.vert
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/17/18
End Header --------------------------------------------------------*/

#version 430 core

uniform vec3 camera;
//texture coords for Kdissuse and Kspecular
// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelTransform;
//model matrix
uniform mat4  modelMatrix;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 modelNormal;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec4 position;
  vec4 normal;
} vs_out;

void main()
{
  vs_out.position = modelMatrix * vec4(modelPosition, 1.0f ); //clip coords
  vs_out.normal = vec4(modelNormal, 0.0f );
  gl_Position = projectionMatrix * viewMatrix * modelTransform * vs_out.position;
}

