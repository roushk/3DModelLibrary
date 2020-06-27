/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: ReflectionMap.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_4
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/24/18
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
uniform float highlightTightness;  //K
uniform float transCoeff;

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;

out vec3 color;

vec4 Refract(float transCoeffpassed)
{

  vec3 cameraPos = camera.xyz;
  vec3 position = fs_in.position.xyz;
  vec3 viewVector = normalize(position - camera); // L = light pos - vert pos
  vec3 finalColor = vec3(0,0,0);

  vec3 normal = fs_in.normal.xyz;
  vec3 newNormal = normal;
  //vec3 cameraPos = (modelPosition * vec4(camera,1)).xyz;

  //R = 2 * (N*V)N - V
  
  float NdotL = dot(newNormal,viewVector);
  //T = [ K*(N*L) - sqrt(1-(k * k)(1-(N*L * N*L)) )] * N - K.L
  vec3 transVector;
  float newTransCoeff = transCoeffpassed;
  if(NdotL < 0) //outside surface
  {
    NdotL = -NdotL;
  }
  else  //inside the surface
  {
    newNormal = -newNormal;

    newTransCoeff = 1.0f/newTransCoeff; //flip coeff cause relationship between outer and inner material is flipped
  }

  float c2 = (sqrt(1.0f - (newTransCoeff * newTransCoeff * (1.0f - (NdotL * NdotL)))));

  //transVector = (newTransCoeff * NdotL - c2) * newNormal - (newTransCoeff * viewVector);
  
  //c1 = NdotL
  //c2 = sqrt(1-(coeff^2)(1-NdotL^2))
  
  transVector = newTransCoeff * viewVector + (newTransCoeff * NdotL - c2) * newNormal;
  //transVector = refract(viewVector, normal, transCoeff);
  transVector = normalize(transVector);
  //LA.lights[i].LightDiffuse.xyz
  //vec3 Irefract = fragColor.xyz * transCoeff * pow(max(dot(viewVector, transVector), 0), highlightTightness);

  //transVector = -transVector;
  vec3 fragColor = vec3(-1,-1,-1);

  vec2 uv = vec2(0,0);

  float absX = abs(transVector.x);
  float absY = abs(transVector.y);
  float absZ = abs(transVector.z);

  bool XPositive = transVector.x > 0 ? true : false;
  bool YPositive = transVector.y > 0 ? true : false;
  bool ZPositive = transVector.z > 0 ? true : false;

  float largestLine = 0;
  int face = -1;
  


  if (XPositive && absX >= absY && absX >= absZ) //X+
  {
    face = 0;
    largestLine = absX;
    uv.x = transVector.z; 
    uv.y = transVector.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) //X-
  {
    face = 1;
    largestLine = absX;
    uv.x = -transVector.z; 
    uv.y = transVector.y;
  }

  if (YPositive && absY >= absX && absY >= absZ) 
  {
    face = 2;
    largestLine = absY;
    uv.x = -transVector.x;
    uv.y = -transVector.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {
    face = 3;
    largestLine = absY;
    uv.x = -transVector.x;
    uv.y = transVector.z;
  }

  if (ZPositive && absZ >= absX && absZ >= absY) //Z+
  {
    face = 4;
    largestLine = absZ;
    uv.x = -transVector.x; 
    uv.y = transVector.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) //Z-
  {
    face = 5;
    largestLine = absZ;
    uv.x = transVector.x; 
    uv.y = transVector.y;
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

  return vec4(fragColor * newTransCoeff * pow(max(dot(viewVector, transVector), 0), highlightTightness), newTransCoeff);
}

vec4 ChromaticAbb()
{
  //color = transVector.xyz;
  float etaR = 0.65f/0.66f;
  float etaG = 0.67f/0.66f;
  float etaB = 0.69f/0.66f;

  vec4 newColor;
  newColor.r = Refract(etaR * transCoeff).r;
  newColor.g = Refract(etaG * transCoeff).g;
  newColor.b = Refract(etaB * transCoeff).b;
  newColor.a = Refract(etaB * transCoeff).a;
  return newColor;
}

void main()
{
  color = ChromaticAbb().rgb;

}
 
 
 