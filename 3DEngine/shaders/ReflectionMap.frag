/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: ReflectionMap.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/17/18
End Header --------------------------------------------------------*/

#version 430 core

// Output data
out vec3 color;

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

layout(binding = 2)uniform sampler2D positiveX;  //face 0
layout(binding = 3)uniform sampler2D negativeX;  //face 1

layout(binding = 4)uniform sampler2D positiveY;  //face 2
layout(binding = 5)uniform sampler2D negativeY;  //face 3

layout(binding = 6)uniform sampler2D positiveZ;  //face 4
layout(binding = 7)uniform sampler2D negativeZ;  //face 5

in VS_OUT
{
  vec4 position;
  vec4 normal;
} fs_in;


void main()
{


  vec3 normal = fs_in.normal.xyz;

  vec3 position = fs_in.position.xyz;
  //vec3 cameraPos = (modelPosition * vec4(camera,1)).xyz;
  vec3 viewVector = normalize(position - camera); // L = light pos - vert pos

  //R = 2 * (N*V)N - V
  vec3 reflectVector = (2.0f *  dot(normal, viewVector) * normal) - viewVector; 

  reflectVector = -reflectVector; //the entire reflection is inverted if i dont


  vec3 fragColor = vec3(-1,-1,-1);

  vec2 uv = vec2(0,0);

  float absX = abs(reflectVector.x);
  float absY = abs(reflectVector.y);
  float absZ = abs(reflectVector.z);

  bool XPositive = reflectVector.x > 0 ? true : false;
  bool YPositive = reflectVector.y > 0 ? true : false;
  bool ZPositive = reflectVector.z > 0 ? true : false;

  float largestLine = 0;
  int face = -1;
  


  if (XPositive && absX >= absY && absX >= absZ) //X+
  {
    face = 0;
    largestLine = absX;
    uv.x = reflectVector.z; 
    uv.y = reflectVector.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) //X-
  {
    face = 1;
    largestLine = absX;
    uv.x = -reflectVector.z; 
    uv.y = reflectVector.y;
  }

  if (YPositive && absY >= absX && absY >= absZ) 
  {
    face = 2;
    largestLine = absY;
    uv.x = -reflectVector.x;
    uv.y = -reflectVector.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {
    face = 3;
    largestLine = absY;
    uv.x = -reflectVector.x;
    uv.y = reflectVector.z;
  }

  if (ZPositive && absZ >= absX && absZ >= absY) //Z+
  {
    face = 4;
    largestLine = absZ;
    uv.x = -reflectVector.x; 
    uv.y = reflectVector.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) //Z-
  {
    face = 5;
    largestLine = absZ;
    uv.x = reflectVector.x; 
    uv.y = reflectVector.y;
  }

  uv.x = 0.5f * (uv.x / largestLine + 1.0f);
  uv.y = 0.5f * (uv.y / largestLine + 1.0f);
  
  


  if(face == 0)
    fragColor = texture(positiveX, uv).rgb; 
  if(face == 1)
    fragColor = texture(negativeX, uv).rgb; 
  if(face == 2)
    fragColor = texture(positiveY, uv).rgb; 
  if(face == 3)
    fragColor = texture(negativeY, uv).rgb; 
  if(face == 4)
    fragColor = texture(positiveZ, uv).rgb; 
  if(face == 5)
    fragColor = texture(negativeZ, uv).rgb; 

  //color = viewVector;

	color = fragColor;
}
 
 
 