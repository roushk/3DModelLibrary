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

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 modelNormal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
//layout(location = 3) in vec3 vertexColor;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
  vec3 tangent;
  vec3 bitangent;

} vs_out;

// Main function - entry point
void main()
{
  vs_out.position = modelTransform * modelMatrix * vec4( modelPosition, 1.0f ); //world coords

  //mat4(transpose(inverse(modelMatrix )))
  vs_out.normal = normalize(modelTransform * modelMatrix * vec4( modelNormal, 0.0f ));
  vs_out.tangent = normalize(vec3(modelMatrix * vec4(tangent, 0.0f)));
  vs_out.bitangent = normalize(vec3(modelMatrix * vec4(bitangent, 0.0f)));

  vs_out.texCoords = texCoords;

  gl_Position = projectionMatrix * viewMatrix * modelTransform * modelMatrix * vec4( modelPosition, 1.0f );  //need position in frag
}
