/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: PhongShading.vert
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430

// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelMatrix;
//model matrix
uniform mat4  modelTransform;

layout(location = 0) in vec3 modelPosition;
layout(location = 2) in vec2 texCoords;

//layout(location = 3) in vec3 vertexColor;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec2 texCoords;
} vs_out;


void main()
{
  gl_Position = projectionMatrix * viewMatrix * modelTransform * modelMatrix * vec4( modelPosition, 1.0f );  //need position in frag
  vs_out.texCoords = texCoords;

} 