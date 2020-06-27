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

#version 430 core

// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelMatrix;
//model matrix
uniform mat4  modelTransform;
//camera position
uniform vec3 camera;

//texture coords for Kdissuse and Kspecular


const int maxLights = 8;
// Uniform blocks
//layout(packed) 
layout(binding = 1)uniform Global
{
  vec4 AttParam;
  vec4 FogColor; 
  vec4 Iglobal; 
  float near;
  float far;
  float Kglobal;  //global ambient color
  int activeLights;
}G;


//layout(packed) 
struct LightArray
{
  vec4 LightAmbient;   //needed for all light types
  vec4 LightDiffuse;
  vec4 LightSpecular;
  vec4 LightPosition;  //depends on the type of light
  vec4 LightDirection;

  int type;

  float outerRadius;
  float innerRadius;
  float falloffValue;
};

layout(std140, binding = 1) uniform LightBlock
{

  LightArray lights[maxLights];

}LA;

// Material values
// These values are hardcoded, but should be read from textures and passed as samplers

uniform vec3 Kemissive; //Emissive: No texture, user specified value [RGB]
uniform vec3 Kambient;  //Use a RGB vector value (specified by user through the GUI)[RGB]
//uniform float ns;     //specular shininess Use square of Ks.r as the shininess value. 

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 modelNormal;
layout(location = 2) in vec2 texCoords;
//layout(location = 3) in vec3 vertexColor;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
} vs_out;

// Main function - entry point
void main()
{
  vs_out.position =  modelMatrix * vec4( modelPosition, 1.0f ); //clip coords

  vs_out.normal = vec4( modelNormal, 0.0f );

  vs_out.texCoords = texCoords;

  gl_Position = projectionMatrix * viewMatrix * modelTransform * vs_out.position;  //need position in frag
}
