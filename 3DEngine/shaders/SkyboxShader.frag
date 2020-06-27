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

// Output data
out vec3 color;


layout(binding = 2)uniform sampler2D positiveX;  //face 0
layout(binding = 3)uniform sampler2D negativeX;  //face 1

layout(binding = 4)uniform sampler2D positiveY;  //face 2
layout(binding = 5)uniform sampler2D negativeY;  //face 3

layout(binding = 6)uniform sampler2D positiveZ;  //face 4
layout(binding = 7)uniform sampler2D negativeZ;  //face 5

in vec3 texCoords;

//uniform mat4 undoCamRotation;


//projectionMatrix * viewMatrix * modelMatrix *
void main()
{
  vec3 position = texCoords;

  //normal = normalize(normal);

  
  vec3 fragColor = vec3(-1,-1,-1);
  
  //posx, negx, posy, negy, posz, negz
  //vec3 newPos = (fs_in.position - R).xyz;
  vec2 uv = vec2(0,0); //fs_in.texCoords; 
  //float face = result.z;


  float absX = abs(position.x);
  float absY = abs(position.y);
  float absZ = abs(position.z);

  bool XPositive = position.x > 0 ? true : false;
  bool YPositive = position.y > 0 ? true : false;
  bool ZPositive = position.z > 0 ? true : false;

  float largestLine = 0;
  int face = -1;
  
  
  //posx, negx, posy, negy, posz, negz
  //vec3 newPos = (fs_in.position - R).xyz;

  if (XPositive && absX >= absY && absX >= absZ) 
  {
    face = 0;
    largestLine = absX;
    uv.x = -position.z;
    uv.y = position.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) 
  {
    face = 1;
    largestLine = absX;
    uv.x = position.z;
    uv.y = position.y;
  }

  if (YPositive && absY >= absX && absY >= absZ) 
  {
    face = 2;
    largestLine = absY;
    uv.x = -position.x;
    uv.y = position.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {
    face = 3;
    largestLine = absY;
    uv.x = position.x;
    uv.y = position.z;
  }
  
  if (ZPositive && absZ >= absX && absZ >= absY) 
  {
    face = 4;
    largestLine = absZ;
    uv.x = position.x;
    uv.y = position.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) 
  {
    face = 5;
    largestLine = absZ;
    uv.x = -position.x;
    uv.y = position.y;
  }

  if(face == 2 || face == 3)
  {
    vec3 uv3D = vec3(uv,1.0f);
    mat3 flipOverXY;
    flipOverXY[0] = vec3(0,-1,0);
    flipOverXY[1] = vec3(-1,0,0);
    flipOverXY[2] = vec3(0,0, 1);
    uv3D = flipOverXY * uv3D;
     
     uv.xy = uv3D.xy;
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

  //fragColor = vec3(0,0,1);//normalize(texCoords);
  color = fragColor;
}
 
 
 