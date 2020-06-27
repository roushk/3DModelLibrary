/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: LightShader.vert
Purpose: Shader for lights face color 
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/2/18
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
//model matrix
uniform mat4  shadowMatrix;

// Material values
// These values are hardcoded, but should be read from textures and passed as samplers

uniform vec3 diffuse; //Emissive: No texture, user specified value [RGB]

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 modelPosition;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec4 position;
} vs_out;


void main()
{
  //projectionMatrix * viewMatrix = shadowMatrix in this shadowShader.vert
  //  vs_out.position = projectionMatrix * viewMatrix * modelTransform * modelMatrix * vec4(modelPosition, 1.0f);

  vs_out.position = shadowMatrix * modelTransform * modelMatrix * vec4(modelPosition, 1.0f);
  gl_Position = vs_out.position;
  
}
