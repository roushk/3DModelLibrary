/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: SkyboxShader.frag
Purpose: Shader for the skybox to map correctly
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/17/18
End Header --------------------------------------------------------*/

#version 430 core

//texture coords for Kdissuse and Kspecular
// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model in world transform
uniform mat4  modelTransform;

//model matrix
uniform mat4  modelMatrix;

// For the first assignment, we will only consider two attributes
// (a) vertex position and
// (b) vertex normal
// We will assign the normal as the color of the fragment (for now)

layout(location = 0) in vec3 modelPosition;

// Interpolating vertex attributes over the rasterizer
out vec3 texCoords;


void main()
{
  gl_Position = projectionMatrix * viewMatrix * modelTransform * modelMatrix * vec4( modelPosition, 1.0f ); //clip coords
  texCoords = (modelMatrix * vec4( modelPosition, 1.0f )).xyz;
}
