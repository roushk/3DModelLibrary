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

uniform sampler2D skydomeTexture;
uniform sampler2D skydomeIRR;

in vec3 normal;
out vec3 color;
uniform float exposure;
uniform float contrast;

//uniform mat4 undoCamRotation;
const float PI = 3.1415926535897932384626433832795;

vec3 sRGBtoLinear(vec3 input, float exposure, float contrast)
{
  return pow(( (exposure * input) / (exposure * input + vec3(1,1,1)) ), vec3(contrast/2.2f));
}


void main()
{

 //had to flip y and z because I use a Y up  
  vec2 uv = vec2((0.5f - ( atan(normal.z, normal.x) / ( 2.0f * PI))), acos(normal.y) / PI);

  color = sRGBtoLinear(texture(skydomeTexture, uv).rgb, exposure, contrast);
  //color = vec3(1,0,0);
}
 
 
 