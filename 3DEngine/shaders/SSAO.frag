/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: PhongShading.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430
//camera position
uniform vec3 camera;


  //(1) choose pairs of uniformly distributed random numbers very carefully
uniform HammersleyBlock 
{
  float N;
  float hammersley[2*100]; 
};

uniform sampler2D gAmbientMap;
uniform sampler2D gPositionMap; //position map.a = emi
uniform sampler2D gNormalMap;
uniform sampler2D gDiffuseMap;
uniform sampler2D gSpecularMap;

in VS_OUT
{
  vec2 texCoords;
} fs_in;

// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelMatrix;
//model matrix
uniform mat4  modelTransform;

//shadow matrix
uniform mat4  shadowMatrix;

const float PI = 3.1415926535897932384626433832795;

const float MSMalpha = 0.001f; //1 x 10^-3

uniform float materialAlpha;

uniform float max_depth;
uniform float scalarLevel;
uniform float exposure;
uniform float contrast;

const int totalSamples = 20;

uniform int height;
uniform int width;

uniform float SSAOcontrast;
uniform float SSAOscale;
uniform float SSAOrange;

out float SSAO_OUT;
//Heaviside step function 
//0 if negative 1 otherwize
//used to exclude stuff outside the range


vec3 worldToNDC(vec3 pos)
{
  vec4 offset = vec4(pos, 1.0);
  offset      = projectionMatrix * viewMatrix * offset;    // from view to clip-space
  offset.xyz /= offset.w;               // perspective divide
  offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0 
  return offset.xyz;
}

int Heaviside(float range, float input)
{
  if(range - input < 0)
  {
    return 0;
  }
  //0 -> inf
  return 1;
}

void main()
{
  //set variables

  //world space positon P and normal N
  vec3 position = texture(gPositionMap, fs_in.texCoords).xyz;
  vec3 normal = texture(gNormalMap, fs_in.texCoords).xyz;

  vec3 cameraPos = camera.xyz;
  
  
  /*
  For a given pixel with a world space position and normal P,N (from the gbuffer), carefully
  choose (as shown in the next section) a selection of n points Pi
  around P within a specified
  range of influence R . The ambient occlusion at P is approximated by summing up the
  occlusion of the n points Pi
  thusly:
 */

  //integer cooefficients (xprime, yprime) from gl_FragCoord.xy
  int xPrime = int(gl_FragCoord.x);
  int yPrime = int(gl_FragCoord.y);

  //floating point coords from xprime/width, yprime/height
  float x = float(gl_FragCoord.x) / width;
  float y = float(gl_FragCoord.y) / height;

  //^ as XOR
  float phi = ((30 * xPrime) ^ yPrime) + (10 * xPrime * yPrime);
  
  //P,N = point and normal at gbuffer (x,y)
  //position = P, normal = N
  //d = camera depth. gbuffer at gbuffer (x,y)
  float d = worldToNDC(texture(gPositionMap, fs_in.texCoords).xyz).z;
  
  //R = world space range of influence R in meters 
  float R = SSAOrange;
  
  float delta = 0.001f;

  //n = numver of points to sample 
  int n = 20; //10 to 20
  float c = 0.1f * R;
  //0 to n - 1
  float total = 0;
  
  for(uint i = 0; i < n; ++i)
  {
    float alpha = (i + 0.5f) / n; //range 0 to 1
    float h = alpha * R / d;
    float theta = 2 * PI * alpha * (7.0f * n / 9.0f) + phi;

    //iPoint = Pi = point in gbuffer at (x,y) + h * (cos(theta), sin(theta))
    //iPoint is hte UV coordinates of Point_i
    vec2 iPoint = fs_in.texCoords + h * vec2(cos(theta), sin(theta));
    vec3 Pi = texture(gPositionMap, iPoint).xyz;
    float Di = worldToNDC(texture(gPositionMap, iPoint).xyz).z;
    vec3 wi = Pi - position;

    total += ( max(0, dot(normal, wi) - (delta * Di)) * Heaviside(R, length(wi))) 
      / max(c * c, dot(wi,wi));
  }

  float S = 2 * PI * c / float(n) * total;
  SSAO_OUT = S;
  SSAO_OUT = max(pow(1.0f - SSAOscale * S,SSAOcontrast),0);
  //SSAO_OUT = 1 - S;
  //SSAO_OUT = SSAOcontrast * SSAOscale;
  //  SSAO_OUT = gl_FragCoord.y * gl_FragCoord.x / float(height * width);
  //SSAO_OUT = y * x;

}
