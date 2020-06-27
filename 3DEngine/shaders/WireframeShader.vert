/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: WireframeShader.vert
Purpose: Simple shader for wireframe objects
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430
// This matrix holds the model-to-view transform
uniform mat4 viewMatrix;

// This matrix holds the projection matrix
uniform mat4 projectionMatrix;

uniform vec4 color; //color of wireframe

layout(location = 0) in vec3 modelPosition;

out VS_OUT
{
  vec4 rasterColor;
} vs_out;

void main()
{
  gl_Position = projectionMatrix * viewMatrix * vec4(modelPosition, 1.0f);
  
  vs_out.rasterColor = color;
}
